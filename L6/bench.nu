
let my_exe = "laplace-acc-gpu"
let binaries = [$my_exe, "laplace-acc",  "laplace-acc-opt", "laplace-seq"]

^nvc++ -acc=gpu -Minfo=acc -fast  -o $my_exe ./laplace-acc.cpp | ignore

$binaries | each {
  let exe = $in
  1..10 | each {
    let out = ^$'./($exe)' | complete | update stdout {lines} | update stderr {lines}
    let time = $out.stdout | parse -r 'Total time was (?<time>[.\d]+) ms' | get -i 0.time | default nan | into float
    {exe: $exe, time: $time} | to json -r | print
  }
} | ignore
