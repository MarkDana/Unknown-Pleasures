import java.util.Arrays;
import java.util.Random;
import java.util.concurrent.*;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.util.LinkedList;
import java.util.List;
import java.util.ArrayList;

public class QuickSort extends RecursiveAction {

    private int array[];
    private int left;
    private int right;
    private static final int THRESHOLD = 10;
    
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

    public static void main(String[] args) throws FileNotFoundException{
    	int [] array;
        if (args[0].equals("-r")){
            int n = Integer.parseInt(args[1]);
            Random random = new Random(0);
            array = new int [n];
            for (int i = 0; i < n; ++i)array[i] = random.nextInt(1000);
        }
        else if (args[0].equals("-i")){
            File arrtxt = new File(args[1]);
            Scanner scnr = new Scanner(arrtxt);
            List<Integer> temps = new ArrayList<Integer>();
            while(scnr.hasNextInt())temps.add(scnr.nextInt());
            array = temps.stream().mapToInt(i->i).toArray();
        }else return;

        System.out.println(Arrays.toString(array));

        ForkJoinPool pool = new ForkJoinPool();
        QuickSort fork = new QuickSort(array,0,array.length-1);
        
        pool.invoke(fork);
        System.out.println(Arrays.toString(array));
    }
}

