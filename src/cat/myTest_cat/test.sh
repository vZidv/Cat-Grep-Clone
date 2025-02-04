#!/bin/bash

FAILS=0
SUCCESSES=0

FLAGS="benstvTE"
TEST_DIR="test_files"

diff_func() {
  local flags=$1
  shift
  local files=("$@")

  if [[ $MODE -eq 1 ]]; then

  DIFF=$(diff <(cat $flags "${files[@]}") <(./../s21_cat $flags "${files[@]}"))
  if [[ -z "$DIFF" ]]; then
    SUCCESSES=$((SUCCESSES + 1))
    echo "SUCCESS $flags ${files[*]}" >>"test.log"
  else
    FAILS=$((FAILS + 1))
    echo "FAIL    $flags ${files[*]}" >>"test.log"
  fi

  elif [[ $MODE -eq 2 ]]; then
  valgrind --leak-check=full --log-file="valgrind_output.log" ./../s21_cat $flags "${files[@]}" > /dev/null 2>$1
    if grep -q "ERROR SUMMARY: 0 errors" valgrind_output.log; then
       SUCCESSES=$((SUCCESSES + 1))
       echo "SUCCESS $flags ${files[*]}" >>"test_memory.log"
    else
    FAILS=$((FAILS + 1))
    echo "FAIL    $flags ${files[*]}" >>"test_memory.log"
    cat valgrind_output.log >> "test_memory.log"
    fi
    rm -f valgrind_output.log
  fi
  }



check_files() {
  local _f=$1
  for text in "$TEST_DIR"/*.txt; do
    diff_func "-$_f" "$text"
  done
  all_texts=($(ls "$TEST_DIR"/*.txt))
  diff_func "-$_f" "${all_texts[@]}"
}

combinations() {
  local str=$1
  local len=$2
  local result=()

  if [[ $len -eq 1 ]]; then
    result=($(echo "$str" | fold -w1))
  else
    for ((i = 0; i <= ${#str} - $len; i++)); do
      local prefix=${str:i:1}
      local suffixes=($(combinations "${str:i+1}" $((len - 1))))
      for s in "${suffixes[@]}"; do
        result+=("$prefix$s")
      done
    done
  fi

  # Возвращаем результат
  echo "${result[@]}"
}

CreatFileLog() {
  local filename=$1

  if [ ! -f "$filename" ]; then
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
#//////////////////////////////////////////////

CheckArgc "$@"
MODE=$1
if [[ $MODE -eq 1 ]]; then
  CreatFileLog "test.log"
elif [[ $MODE -eq 2 ]]; then
  CreatFileLog "test_memory.log"
fi

# Для каждой длины генерируем сочетания
for k in $(seq 1 ${#FLAGS}); do
  combs=($(combinations "$FLAGS" $k))
  for comb in "${combs[@]}"; do
    check_files "$comb"
  done
done

check_files "-number"
check_files "-number-nonblank"
check_files "-number --number-nonblank -stE"
NUM_ALL=$((FAILS + SUCCESSES))
echo "Number of tests: $NUM_ALL"
echo "Number of fails: $FAILS"
echo "Number of successes: $SUCCESSES"
