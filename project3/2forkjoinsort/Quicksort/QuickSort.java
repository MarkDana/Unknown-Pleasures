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
    }

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
        System.out.println(String.format("Pivot index of [%d,%d] is %d",left,right,l));
        return l;
    }

    public static void main(String[] args) {
    	int[] array = new int[200];

        for (int i = 0; i < 200; i++) {
            array[i] = new Random().nextInt(1000);
        }

        System.out.println(Arrays.toString(array));

        ForkJoinPool pool = new ForkJoinPool();
        MergeSort fork = new MergeSort(array,0,array.length-1);
        
        pool.invoke(fork);
        System.out.println(Arrays.toString(array));
    }
}

