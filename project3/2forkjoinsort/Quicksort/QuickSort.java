import java.util.Arrays;
import java.util.Random;
import java.util.concurrent.*;
import java.util.Scanner;

public class QuickSort extends RecursiveAction {

    private int array[];
    private int left;
    private int right;
    private static final int THRESHOLD = 100;
    
    QuickSort(int[] array, int left, int right) {
        this.array = array;
        this.left = left;
        this.right = right;
        System.out.println(String.format("Creating sub-sort %d to %d",left,right));
    }

    public void compute() {
	int size = right - left + 1;
        if (size <= 1)return;
        if (size <= THRESHOLD){
            Arrays.sort(array, left, right + 1);
            return;
        }

        int pi = pivotIndex();
        QuickSort sortLeft = new QuickSort(array, left, pi);
        QuickSort sortRight = new QuickSort(array, pi+1, right);
        
        sortLeft.fork();
        sortRight.fork();
        sortLeft.join();
        sortRight.join();

    private int pivotIndex() {
        int pivot = array[left];
        int l = left, r = right;
        while(l < r){
            while(l < r && array[r] >= pivot) --r;
            array[l] = array[r];
            while(l < r && array[l] <= pivot) ++l;
            array[r] = array[l];
        }
        array[l] = pivot;
        return l;
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
    QuickSort task=new QuickSort(array, 0, array.length-1);
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

