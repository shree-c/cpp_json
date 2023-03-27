totalc=1
exe_dir=$(dirname ${BASH_SOURCE[0]})
passed=0
stdout_file="./$exe_dir/../results/std_cases_stdout"
stderr_file="./$exe_dir/../results/std_cases_stderr"
echo '' > passed
echo '' > failed
for F in "./$exe_dir/../test_cases/"*;
do 
  fn=$(basename $F)
  cat $F | "./$exe_dir/../bin/json_cpp" >> $stdout_file 2>> $stderr_file; 
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

echo total = $totalc passed = $passed failed $(($totalc - $passed))
