

def generate_tests [] {
  let height_range = 128..160.. | take 100
  let width_range = 128..160.. | take 100
  
  let height_test = $height_range | par-each {
    {height: $in, width: 1024, test: height}
  }
  let width_test = $width_range | par-each {
    {height: 1024, width: $in, test: width}
  }
  $height_test | append $width_test
}

def run_test [] {
  let config = $in
  let tmp_dir = mktemp -d
  let exe = $tmp_dir | path join exe
  let compile_result = nvc++ -acc=gpu -Minfo=acc -fast -DHEIGHT=($config.height) -DWIDTH=($config.width) -o $exe ./laplace-acc.cpp | complete | update stdout {lines} | update stderr {lines}
  
  if $compile_result.exit_code != 0 {
    rm $tmp_dir
    return $config | insert compile $compile_result
  }

  let runs = 1..5 | each {
    let output = ^($exe) | complete | update stdout {lines} | update stderr {lines}
    let time = $output.stdout | parse -r 'Total time was (?<time>[.\d]+) ms' | get -i 0.time | default NaN | into float

    $output | insert time $time
  }

  $config | insert compile $compile_result | insert runs $runs | to json -r | print

  rm -r $tmp_dir
}

def main [] {
  generate_tests | each {run_test} | ignore
}
