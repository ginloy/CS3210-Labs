#!/usr/bin/env bash

TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' EXIT

EXE="$(pwd)/mm"
STDERR_PIPE="$TMP_DIR/stderr"
STDOUT_PIPE="$TMP_DIR/stdin"
mkfifo "$STDERR_PIPE" "$STDOUT_PIPE"

>&2 mpic++ -o "$EXE" ./mm-mpi.cpp

format_output() {
  if [[ $# -ne 3 ]]; then
    >&2 echo "3 arguments needed, received $#"
    return 1
  fi
  local stdout
  local stderr
  local workers
  local tmp_dir
  local stdout_pipe
  local stderr_pipe
  tmp_dir=$(mktemp -d)
  trap 'rm -rf "$tmp_dir"' RETURN
  stdout_pipe="$tmp_dir/stdout"
  stderr_pipe="$tmp_dir/stderr"
  mkfifo "$stdout_pipe" "$stderr_pipe"
  for run in $(seq 1 5); do
    eval "$1" >"$stdout_pipe" 2>"$stderr_pipe" &
    exec 3<"$stdout_pipe" 4<"$stderr_pipe"
    stdout=$(cat <&3)
    stderr=$(cat <&4)
    exec 3<&- 4<&-
    if [[ $stdout ]]; then
      >&2 echo "$stdout"
    fi
    >&2 echo "$stderr"
    total_time=$(sed -E -n 's/^.*total time= +([0-9]+[.][0-9]+).*$/\1/p' <<<"$stderr")
    workers=$(sed -E -n \
      's/^.*WORKER +([0-9]+) +\(on (.*)\): +communication_time= +([0-9]+[.][0-9]+).*computation_time= +([0-9]+[.][0-9]+).*$/\1,\2,\3,\4/p' \
      <<<"$stderr")
    for row in $workers; do
      echo "$2,$3,$run,$row,$total_time"
    done
  done
}

echo 'test_type,partition,run,worker,node,communication_time,computation_time,total_time'
format_output "srun -p i7-7700 -N 1 -n 5 --cpu-bind=thread $EXE" A i7-7700 &
format_output "srun -p xs-4114 -N 1 -n 5 --cpu-bind=thread $EXE" A xs-4114 &
format_output "srun --constraint='[i7-7700*1&xs-4114*1]' --distribution=cyclic -N 2 -n 5 --cpu-bind=thread $EXE" A both &
format_output "srun -n5 -N1 -p i7-7700 $EXE 256" Bn5 i7-7700 &
format_output "srun -n9 -N1 --overcommit -p i7-7700 $EXE 256" Bn9 i7-7700 &

wait
