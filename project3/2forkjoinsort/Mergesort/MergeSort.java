import java.util.Arrays;
import java.util.Random;
import java.util.concurrent.*;

public class MergeSort extends RecursiveAction {
    //eg. [x,y,z], left=0, right=2
    private int array[];
    private int left;
    private int right;
    private static final int THRESHOLD = 3;

    MergeSort(int[] array, int left, int right) {
        this.array = array;
        this.left = left;
        this.right = right;
        System.out.println(String.format("creating sort %d to %d",left,right));
    }

    public void compute() {
        int size = right - left + 1;
        System.out.println(String.format("from %d to %d",left,right));
        if (size <= 1)return;
        if (size <= THRESHOLD){
            Arrays.sort(array, left, right + 1);
            return;
        }//in java util, pass in the next index of ending

        int mid = left + size >> 1;
        MergeSort sortLeft = new MergeSort(array, left, mid);
        MergeSort sortRight = new MergeSort(array, mid+1, right);
        
        sortLeft.fork();
        sortRight.fork();
        sortLeft.join();
        sortRight.join();
        merge(mid);
    }

    public void merge(int mid) {
        int[] temp = new int[right - left + 1];
        int i = left, j = mid + 1, k = 0;
        while(i <= mid && j <= right){
            if(array[i] <= array[j])temp[k++] = array[i++];
            else temp[k++] = array[j++];
        }
        while(i <= mid) temp[k++] = array[i++];
        while(j <= right) temp[k++] = array[j++];
        for (k = left; k <= right; ++k)array[left + k] = temp[k];
    }

    public static void main(String[] args) {

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