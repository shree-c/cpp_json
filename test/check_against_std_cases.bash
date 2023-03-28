totalc=1
exe_dir=$(dirname ${BASH_SOURCE[0]})
passed=0
stdout="./$exe_dir/../results/std_cases_stdout"
stderr="./$exe_dir/../results/std_cases_stderr"
test_status="./$exe_dir/../results/test_status"
truncate -s 0 $stdout_file $stdout_file passed failed
for F in "./$exe_dir/../test_cases/"*;
do 
  fn=$(basename $F)
  cat $F | "./$exe_dir/../bin/json_cpp" >> $stdout 2>> $stderr; 
  x=$?
  totalc=$(($totalc + 1))
  if [[ $fn == n* ]]; then
    if [ $x -eq 0 ]; then
      echo $fn ❌;
      echo $fn >> failed;
    else
      echo $fn ✅ ;
      passed=$(($passed + 1));
      echo $fn >> passed
    fi
  elif [[ $fn == y* ]] || [[ $fn == i* ]]; then
    if [ $x -eq 0 ]; then
      echo $fn ✅;
      passed=$(($passed + 1));
      echo $fn >> passed;
    else
      if [[ $fn == i* ]]; then
        passed=$(($passed + 1))
      fi
      echo $fn ❌;
      echo $fn >> failed
    fi
  fi
done

status="total = $totalc passed = $passed failed $(($totalc - $passed))"

echo $status

echo $status >> $test_status
