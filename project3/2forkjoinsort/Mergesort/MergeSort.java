import java.util.Arrays;
import java.util.Random;
import java.util.concurrent.*;
import java.util.Scanner;
import java.io.File;
import java.io.FileNotFoundException;

public class MergeSort extends RecursiveAction {
    //eg. [x,y,z], left=0, right=2
    private int array[];
    private int left;
    private int right;
    private static final int THRESHOLD = 100;

    MergeSort(int[] array, int left, int right) {
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
        }//in java util, pass in the next index of ending

        int mid = (left+right)>>1;
        MergeSort sortLeft = new MergeSort(array, left, mid);
        MergeSort sortRight = new MergeSort(array, mid+1, right);
        
        sortLeft.fork();
        sortRight.fork();
        sortLeft.join();
        sortRight.join();
        merge(mid);
    }

    public void merge(int mid) {
        System.out.println(String.format("Mrerging sorted [%d,%d] and [%d,%d]",left,mid,mid+1,right));
        int[] temp = new int[right - left + 1];
        int i = left, j = mid + 1, k = 0;
        while(i <= mid && j <= right){
            if(array[i] <= array[j])temp[k++] = array[i++];
            else temp[k++] = array[j++];
        }
        while(i <= mid) temp[k++] = array[i++];
        while(j <= right) temp[k++] = array[j++];
        for (k = left; k <= right; ++k)array[k] = temp[k-left];
    }

    public static void main(String[] args) {

        // System.out.println("Working Directory = " +
        //       System.getProperty("."));

        // System.out.println(new File("."));

        
        // System.out.println(new File(args[0]).getAbsolutePath());
        
        File arrtxt = new File("array.txt");
        // System.out.println("array.txt".getAbsolutePath());
        Scanner scnr = new Scanner(arrtxt);
        // int [] array = new int [1000];
        // int i = 0;
        // while(scanner.hasNextInt())array[i++] = scanner.nextInt();
        


        int[] array = new int[20];

        for (int i = 0; i < 20; i++) {
            array[i] = new Random().nextInt(100);
        }

        System.out.println(Arrays.toString(array));

        ForkJoinPool pool = new ForkJoinPool();
        MergeSort fork = new MergeSort(array,0,array.length-1);
        
        pool.invoke(fork);
        System.out.println(Arrays.toString(array));
    }
}