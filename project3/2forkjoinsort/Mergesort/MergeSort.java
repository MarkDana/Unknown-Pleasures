import java.util.Arrays;
import java.util.Random;
import java.util.concurrent.*;

class Main extends RecursiveAction {
    private static final int SERIAL_THRESHOLD = 100;
    private int array[];
    private int left;
    private int right;

    public Main(int[] array, int left, int right) {
        this.array = array;
        this.left = left;
        this.right = right;
    }

    /**
     * Inherited from RecursiveAction.
     * Compare it with the run method of a Thread.
     */
    @Override
    protected void compute() {
        if (left < right) {
            if (serialThresholdMet()) {
                Arrays.sort(array, left, right + 1);
            }
            else{
                int mid = (left + right) / 2;
                Main leftSort = new Main(array, left, mid);
                Main rightSort = new Main(array, mid + 1, right);
                invokeAll(leftSort, rightSort);
                merge(left, mid, right);
            }
        }
    }

    private boolean serialThresholdMet() {
        return right - left < SERIAL_THRESHOLD;
    }

    private void merge(int left, int mid, int right) {
        int temp [] = new int[right - left + 1];
        int x = left;
        int y = mid + 1;
        int z = 0;
        while (x <= mid && y <= right) {
            if (array[x] <= array[y]) {
                temp[z] = array[x];
                z++;
                x++;
            } else {
                temp[z] = array[y];
                z++;
                y++;
            }
        }
        while (y <= right) {
            temp[z++] = array[y++];
        }
        while (x <= mid) {
            temp[z++] = array[x++];
        }

        for (z = 0; z < temp.length; z++) {
            array[left + z] = temp[z];
        }
    }

    public static void main(String[] args) {

        int[] array = new int[20];

        for (int i = 0; i <= 19; i++) {
            array[i] = new Random().nextInt(100);
        }

        System.out.println(Arrays.toString(array));

        Main fork = new Main(array,0,array.length-1);
        ForkJoinPool pool = new ForkJoinPool();
        pool.invoke(fork);
        System.out.println(Arrays.toString(array));
    }
}