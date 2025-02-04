#!/bin/bash

FAILS=0
SUCCESSES=0

TEST_DIR="test_files"
FLAGS="lcovnhsief"
TEST_WORDS=("f" "hope" "birds")

diff_func() {
  local flags=$1
  local findWord=$2
  shift 2
  local files=("$@")

  if [[ $MODE -eq 1 ]]; then
    #echo "Runnung: grep $flags $findWord ${files[@]}"
    #echo "Runnung: ./../s21_grep $flags $findWord ${files[@]}"
    DIFF=$(diff <(grep $flags "$findWord" "${files[@]}" "-E") <(./../s21_grep $flags "$findWord" "${files[@]}"))
    if [[ -z "$DIFF" ]]; then
      SUCCESSES=$((SUCCESSES + 1))
      echo "SUCCESS $flags $findWord ${files[*]}" >>"test.log"
    else
      FAILS=$((FAILS + 1))
      echo "FAIL    $flags $findWord ${files[*]}" >>"test.log"
    fi

  elif [[ $MODE -eq 2 ]]; then
    valgrind --leak-check=full --log-file="valgrind_output.log" ./../s21_grep "$findWord" $flags "${files[@]}" >/dev/null 2>valgrind_output.log
    if grep -q "ERROR SUMMARY: 0 errors" valgrind_output.log; then
      SUCCESSES=$((SUCCESSES + 1))
      echo "SUCCESS $flags $findWord ${files[*]}" >>"test_memory.log"
    else
      FAILS=$((FAILS + 1))
      echo "FAIL    $flags $findWord ${files[*]}" >>"test_memory.log"
      cat valgrind_output.log >>"test_memory.log"
    fi
    rm -f valgrind_output.log
  fi
}

CheckFile() {
  local _f=$1
  local findWord=$2

  if [[ "$_f" == *"e"* ]]; then
    diff_func "-e" "$findWord" "$TEST_DIR"/*.txt
    return
  fi
  if [[ "$_f" == *"f"* ]]; then
    diff_func "-f" "FindWords/Words.txt" "$TEST_DIR"/*.txt
    return
  fi
  for text in "$TEST_DIR"/*.txt; do
    diff_func "-$_f" "$findWord" "$text"
  done
  all_texts=($(ls "$TEST_DIR"/*.txt))
  diff_func "-$_f" "$findWord" "${all_texts[@]}"
}
Combinations() {
  local str=$1
  local len=$2
  local result=()

  if [ $len -eq 1 ]; then
    result=($(echo "$str" | fold -w1))
  else
    for ((i = 0; i < ${#str}; i++)); do
      local prefix=${str:i:1}
      local suffixes=$(Combinations "${str:i+1}" $((len - 1)))
      for s in $suffixes; do
        result+=("$prefix$s")
      done
    done
  fi

  echo "${result[@]}"
}

CreatFileLog() {
  local filename=$1

  if [[ ! -f "$filename" ]]; then
    touch "$filename"
  else
    >"$filename"
  fi
}

CheckArgc() {
  if [ $# -ne 1 ]; then
    echo "Использование: $0 <режим работы>."
    echo "1 - обычная проверка. 2 - проверка на утечек памяти."
    exit 1
  fi
  if [[ $1 -ne 1 && $1 -ne 2 ]]; then
    echo "Неверный режим работы: Используйте 1 или 2."
    echo "1 - обычная проверка. 2 - проверка на утечек памяти."
    exit 1
  fi
}
#//////////////////////////////////////////////////////
CheckArgc "$@"
MODE=$1

if [[ $MODE -eq 1 ]]; then
  CreatFileLog "test.log"
elif [[ $MODE -eq 2 ]]; then
  CreatFileLog "test_memory.log"
fi

for k in $(seq 1 ${#FLAGS}); do
  combs=($(Combinations "$FLAGS" $k))
  for comb in "${combs[@]}"; do
    for word in "${TEST_WORDS[@]}"; do
      CheckFile "$comb" "$word"
    done
  done
done

NUM_ALL=$((FAILS + SUCCESSES))
echo "Number of tests: $NUM_ALL"
echo "Number of fails: $FAILS"
echo "Number of successes: $SUCCESSES"
