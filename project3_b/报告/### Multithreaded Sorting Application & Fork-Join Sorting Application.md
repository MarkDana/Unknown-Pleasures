### Multithreaded Sorting Application & Fork-Join Sorting Application

## 问题

Write a multithreaded sorting program that works as follows: Alist of integers is divided into two smaller lists of equal size. Two separate threads (which we will term sorting threads) sort each sublist using a sorting algorithm of your choice. The two sublists are then merged by a third thread—a merging thread —which merges the two sublists into a single sorted list.

Implement the preceding project (Multithreaded Sorting Application) using Java’s fork-join parallelism API. This project will be developed in two different versions. Each version will implement a different divide-and-conquer sorting algorithm

1. Quicksort
2. Mergesort

The Quicksort implementation will use the Quicksort algorithm for dividing the list of elements to be sorted into a left half and a right half based on the two evenly sized halves. For both the quicksort and Mergesort algorithms, when the list to be sorted falls within some threshold value (for example, the list is size 100 or fewer), directly apply a simple algorithm such as the Selection
or Insertion sort. Most data structures texts describe these two well-known, divide-and-conquer sorting algorithms. The class SumTask shown in Section 4.5.2.1 extends RecursiveTask,which is a result-bearing ForkJoinTask. As this assignment will involve sorting the array that is passed to the task, but not returning any values, you will instead create a class that extends RecursiveAction, a non result-bearing ForkJoinTask (see Figure 4.19). The objects passed to each sorting algorithm are required to implement Java’s Comparable interface, and this will need to be reflected in the class
definition for each sorting algorithm. The source code download for this text includes Java code that provides the foundations for beginning this project.

## 解决方案

### Quicksort

快速排序原理：

1. 获得待排序数组a
2. 选取一个合适的数字p(一般来说就选取数组或是子数组的第一个元素)作为排序基准
3. 将待排序数组a中比基准p小的放在p的左边，比基准p大的放在p的右边 
4. 从第3步获得的两个子数组sub1跟sub2
5. 判断sub1或sub2中是否只有一个元素，如果只有一个元素则返回此元素，否则就将sub1（或是sub2）代回到第1步中继续执行

按照题目中的提示，调用java的API：RecursiveAction。RecursiveAction 是 ForkJoinTask 的一个子类，它代表了一类最简单的 ForkJoinTask：不需要返回值，当子任务都执行完毕之后，不需要进行中间结果的组合。如果我们从 RecursiveAction 开始继承，那么我们只需要重载 `protected void compute()` 方法。重载的函数如下：

```c
    protected void compute() {
        if (serialThresholdMet()) {
          Arrays.sort(a, left, right + 1);
        } else {
            int pivotIndex = partition(a, left, right);
            ForkJoinTask t1 = null;

            if (left < pivotIndex)
                t1 = new Main(a, left, pivotIndex).fork();
            if (pivotIndex + 1 < right)
                new Main(a, pivotIndex + 1, right).invoke();

            if (t1 != null)
                t1.join();
        }
    }
```
首先通过 `partition()` 方法将数组分成两个部分。随后，两个子任务将被生成并分别排序数组的两个部分。当子任务足够小时，再将其分割为更小的任务反而引起性能的降低。因此，这里我们使用一个阈值，限定在子任务规模较小时，使用直接排序，而不是再将其分割成为更小的任务。其中，我们用到了 RecursiveAction 提供的方法 `coInvoke()`。它表示：启动所有的任务，并在所有任务都正常结束后返回。如果其中一个任务出现异常，则其它所有的任务都取消。`coInvoke()` 的参数还可以是任务的数组。`ForkJoinPool()` 默认建立具有与 CPU 可使用线程数相等线程个数的线程池。我们在一个 JUnit 的 `test` 方法中将Quicksort提交给一个新建的 ForkJoinPool。最后建立一个随机数组，输入算法进行排序。

### Mergesort
归并将两个或两个以上的有序表合并成一个新的有序表。如下图所示，有两个已经排好序的有序表A[1]~A[n]和B[1]~B[m]（在图中只给出了它们的关键字），通过归并把它们合成一个有序表C[1]~C[m+n]。

<img src="C:\Users\lenovo\Desktop\1.png" style="zoom:80%" />

基本思想：归并（Merge）排序法是将两个（或两个以上）有序表合并成一个新的有序表，即把待排序序列分为若干个子序列，每个子序列是有序的。然后再把有序子序列合并为整体有序序列。

方法：
设r[i…n]由两个有序子表r[i…m]和r[m+1…n]组成，两个子表长度分别为n-i +1、n-m。

1. j=m+1；k=i；i=i; //置两个子表的起始下标及辅助数组的起始下标
2. 若i>m 或j>n，转⑷ //其中一个子表已合并完，比较选取结束
3. //选取r[i]和r[j]较小的存入辅助数组rf
如果r[i]<r[j]，rf[k]=r[i]； i++； k++； 转⑵
否则，rf[k]=r[j]； j++； k++； 转⑵
4. //将尚未处理完的子表中元素存入rf
如果i<=m，将r[i…m]存入rf[k…n] //前一子表非空
如果j<=n ,  将r[j…n] 存入rf[k…n] //后一子表非空
5. 合并结束。
归并排序同样调用RecursiveAction，重载compute函数，但是需要注意的是，这里采用invokeall函数进行归并。这是`ForkJoinTask`的一个方法，代码如下：

```java
public static void invokeAll(ForkJoinTask<?> t1, ForkJoinTask<?> t2) {
    int s1, s2;
    t2.fork();
    if ((s1 = t1.doInvoke() & DONE_MASK) != NORMAL)
        t1.reportException(s1);
    if ((s2 = t2.doJoin() & DONE_MASK) != NORMAL)
        t2.reportException(s2);
}
```
代码执行顺序如下：
1，先执行 t2 的 fork 方法，把 t2 这个 task 加到 workQueue 里去。 
2，再执行 t1 的 doInvoke 方法，让 t1 同步执行。 
3，当 t1 执行完后，执行 t2 的 doJoin 方法，执行 t2 任务。
重载的compute函数如下：

```java
    @Override
    protected void compute() {
        if (serialThresholdMet()) {
          Arrays.sort(a, left, right + 1);
        } else {
            int pivotIndex = partition(a, left, right);
            ForkJoinTask t1 = null;

            if (left < pivotIndex)
                t1 = new Main(a, left, pivotIndex).fork();
            if (pivotIndex + 1 < right)
                new Main(a, pivotIndex + 1, right).invoke();

            if (t1 != null)
                t1.join();
        }
    }
```

## 运行结果
20个随机数的排序
Quicksort

<img src="C:\Users\lenovo\Desktop\quick.jpg" style="zoom:80%" />

Mergesort

<img src="C:\Users\lenovo\Desktop\merge.jpg" style="zoom:80%" />