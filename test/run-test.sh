
PARAMS=-O3
LIBS=-lc
COMPILER=../build/bin/tumble

passed_count=0
failed_count=0

function runTest {
    if [ ${1: -4} == .tbl ]
    then
        for i in $(seq 1 $2)
        do
            echo -n "  "
        done
        if $COMPILER $PARAMS -o /tmp/tumble-test.tmp.out $1 $LIBS &> /dev/null && [ -f /tmp/tumble-test.tmp.out ]
        then
            if timeout 0.5s /tmp/tumble-test.tmp.out &> /dev/null
            then
                echo -e "\e[32mPassed\e[m test '$3'"
                passed_count=$(expr $passed_count + 1)
            else
                echo -e "\e[31mFailed\e[m test '$3' at runtime ($?)"
                failed_count=$(expr $failed_count + 1)
            fi
        else
            echo -e "\e[31mFailed\e[m test '$3' at compilation"
            failed_count=$(expr $failed_count + 1)
        fi
        rm /tmp/tumble-test.tmp.out &> /dev/null
    fi
}

function runTests {
    for i in $(seq 1 $2)
    do
        echo -n "  "
    done
    echo "Running tests in: '$3'"
    for test in $(find $1 -mindepth 1 -maxdepth 1)
    do
        name=$(awk -F/ '{print $NF}' <<< $test)
        if [ ${name::7} != ignore. ]
        then
            if [ -f $test ]
            then
                runTest $test $(expr $2 + 1) $name
            elif [ -d $test ]
            then
                runTests $test $(expr $2 + 1) $name
            fi
        fi
    done
}

if [ $1 -a -d $1 ]
then
    echo
    path=$(realpath $1)
    name=$(awk -F/ '{print $NF}' <<< $path)
    runTests $path 1 $name
    echo

    echo -e "\e[32mPassed $passed_count tests\e[m"
    echo -e "\e[31mFailed $failed_count tests\e[m"
    if [ $failed_count -gt 0 ]
    then
        exit 1
    else
        exit 0
    fi
else
    echo "Usage: $0 <test-dir>"
    exit 2
fi

