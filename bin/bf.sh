


if [ $# -lt 2 ] ; then
    echo "Useage: bf <mode> <input-file> -[s(ize)][o(utput)]";
else


    BIN="${BASH_SOURCE[0]}";
    if ([ -h "${BIN}" ]) then
      while([ -h "${BIN}" ]) do BIN=`readlink "${BIN}"`; done
    fi
    pushd . > /dev/null;
    cd `dirname ${BIN}` > /dev/null;
    BIN=`pwd`;
    popd  > /dev/null;

    export PATH=$PATH:$BIN;


    if [ "$1" == "run" ] || [ "$1" == "r" ] ; then
        bfi "$2";
    elif [ "$1" == "compile" ] || [ "$1" == "c" ] ; then

        datasize="30000";
        if [ "$3" == "-size" ] || [ "$3" == "-s" ] ; then
            datasize="$4";
        elif [ "$5" == "-size" ] || [ "$5" == "-s" ] ; then
            datasize="$6";
        fi

        output="none";

        if [ "$3" == "-output" ] || [ "$3" == "-o" ] ; then
            output="-o"$4"";
        elif [ "$5" == "-output" ] || [ "$5" == "-o" ] ; then
            output="-o"$6"";
        fi


        bfc "$2" "tmp.c" "$datasize";

        if [ "$output" == "none" ] ; then
            gcc "tmp.c";
        else
            gcc "tmp.c" "$output";
        fi
        rm "tmp.c";
    fi
fi
