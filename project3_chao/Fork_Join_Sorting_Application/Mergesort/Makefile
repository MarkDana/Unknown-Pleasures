All: 
	javac MergeSort.java 
clean:
	rm *.class



# 原来由于pthread库不是Linux系统默认的库，连接时需要使用库libpthread.a,所以在使用pthread_create创建线程时，在编译中要加-lpthread参数:
# gcc -o test -lpthread test.c   这样好像不成  改成pthread才可以
# 再查发现编译时参数写成 -pthread 也是可以的。
# ————————————————
# 版权声明：本文为CSDN博主「FinnAwe」的原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接及本声明。
# 原文链接：https://blog.csdn.net/liangsir_l/article/details/50638739
