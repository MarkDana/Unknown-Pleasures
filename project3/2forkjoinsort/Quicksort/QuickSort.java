import java.util.Arrays;
import java.util.Random;
import java.util.concurrent.*;
import java.util.Scanner;

public class QuickSort extends RecursiveAction {

    private final int[] array; //排序的数组，由于传进来的数组在堆里，而且是引用传递，所以这个数组是所有线程共享的
    private final int low; // 需要排序的最低一位
    private final int high; //需要排序的最高一位的后一位
    private final int threshold=128; //低于这个阈值，不需要再归并排序，直接排序即可
    
    QuickSort(int[] array, int low, int high) {
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
	else {
        int middle=divide(); //进行划分
        QuickSort leftTask = new QuickSort(array, low, middle-1);
        QuickSort rightTask = new QuickSort(array, middle+1,high);
        leftTask.fork(); // 对左边进行快速排序
        rightTask.fork(); //对右边进行快速排序
        leftTask.join(); // 等待左边快速排序执行完
        rightTask.join(); // 等待右边快速排序执行完
    
	}
    System.out.println(String.format("第%d位到第%d位排序完毕",low,high-1));
    }

    private int divide() { // 划分
        int pivot=array[low]; // 找出划分元素
        int middle=low;
        int l=low,r=high-1;
        while(l<r){
            while(l<r && array[r]>=pivot) --r; //比划分元素大，留在右边
                array[l]=array[r]; //移到左边
            while(l<r && array[l]<=pivot) ++l;; // 比划分元素小，留在左边
                array[r]=array[l]; //移到右边
        }
    array[l]=pivot; // 放置划分元素
    return l; //返回所在位置
	
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
    QuickSort task=new QuickSort(array, 0, array.length);
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

