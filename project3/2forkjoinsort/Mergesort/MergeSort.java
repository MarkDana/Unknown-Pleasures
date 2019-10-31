import java.util.Arrays;
import java.util.Random;
import java.util.concurrent.*;
import java.util.Scanner;

public class MergeSort extends RecursiveAction {

    private final int[] array; //排序的数组，由于传进来的数组在堆里，而且是引用传递，所以这个数组是所有线程共享的
    private final int low; // 需要排序的最低一位
    private final int high; //需要排序的最高一位的后一位
    private final int threshold=128; //低于这个阈值，不需要再归并排序，直接排序即可
    
    MergeSort(int[] array, int low, int high) {
	this.array = array;
	this.low = low;
	this.high = high;
    }

    public void compute() {
	int size = high - low;
    System.out.println(String.format("本线程负责第%d位到第%d位的数字",low,high-1));
	if (size <= threshold) { // 直接排序
	    Arrays.sort(array, low, high);
	}
	else { //分治 
        int middle = low + (size >> 1);
        MergeSort leftTask = new MergeSort(array, low, middle); 
        MergeSort rightTask = new MergeSort(array, middle, high);
        leftTask.fork(); //前一半进行归并排序
        rightTask.fork(); //对后一半进行归并排序
        leftTask.join(); //等待执行完成
        rightTask.join();
        merge(middle); //把结果进行合并
	}
    System.out.println(String.format("第%d位到第%d位排序完毕",low,high-1));
    }

    private void merge(int middle) {
	if (array[middle - 1] < array[middle]) { //不需要合并的情况
	    return;
	}
    int copySize = high - low;
    int[] copy=new int[copySize]; //开辟一个新数组
    int i=low,j=middle,k=0;
    while(i<middle && j<high){ // 合并到新数组里面
        if(array[i]<=array[j]) copy[k++]=array[i++];
        else copy[k++]=array[j++];
    }
    while(i<middle) copy[k++]=array[i++];
    while(j<high) copy[k++]=array[j++];
    for(i=low;i<high;++i) //拷贝回来
        array[i]=copy[i-low];
    }

     public static void main(String[] args) {
	int n = Integer.parseInt(args[0]);
    System.out.println(String.format("一共随机生成了%d个数字",n));
	Random random = new Random(0);
	int[] array = new int[n];
	for (int i = 0; i < n; ++i) {
	    array[i] = random.nextInt();
	}
	
    ForkJoinPool pool = new ForkJoinPool();
    MergeSort task=new MergeSort(array, 0, array.length);
    pool.invoke(task);
    // ForkJoinPool invoke、execute和submit区别 https://blog.csdn.net/Thousa_Ho/article/details/89164259
    System.out.println();
    Scanner sc = new Scanner(System.in); 
    System.out.println("是否输入排序后的结果? 0 表示不输出 1表示输出"); 
    String content = sc.nextLine();
    int choose = Integer.parseInt(content); 
    if(choose!=0){
        for(int i=0;i<n;++i)  System.out.println(array[i]); 
    }

    }
}

