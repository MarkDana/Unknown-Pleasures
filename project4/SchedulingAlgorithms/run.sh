#! /bin/bash 
execute(){
    declare -A map=(["1"]="fcfs" ["2"]="sjf" ["3"]="priority" ["4"]="rr" ["5"]="priority_rr") 
    if [ "$#" -eq 0 ];then 
        choice=1
    else
        choice=$1
    fi
    dirname=$(pwd)
    echo -e ""
    echo -e "现在开始编译${map[${choice}]}调度算法文件,编译完成后自动执行"
    make ${map[${choice}]}
    ${dirname}/${map[$choice]} "schedule.txt"

}

main(){
    echo -e "现在删除之前编译的文件"
    make clean
    read -p "是否要顺序执行?y代表顺序执行5个调度算法,n代表指定哪个调度算法(y/n):    " option
    [ -z ${option} ] && option="y"
    if [ "${option}" == "y" ] || [ "${option}" == "Y" ]
    then
        for ((i=1; i<=5; i++))
        do
            execute $i
            if [ "${i}" -ne 5 ];then
                read -p "是否继续顺序执行?y代表继续顺序执行调度算法,n代表指定哪个调度算法(y/n):    " option
                if [ "${option}" == "n" ] || [ "${option}" == "N" ];then
                    break
                fi
            else
                echo -e "5个算法已执行完毕"
            fi
        done
    fi
    if [ "${option}" == "n" ] || [ "${option}" == "N" ]
    then 
        while true
        do
           read -p "请输入要执行的调度算法,0代表退出,1代表fcfs,2代表sjf,3代表priority,4代表rr,5代表priority_rr:    " choice
            if [ "${choice}" -eq 0 ];then
                break
            fi
            if [ "${choice}" -ge 1 ] && [ "${choice}" -le 5 ];then
                execute ${choice}
            else 
                echo -e "数字输入错误!"
            fi
        done
    fi
}
main