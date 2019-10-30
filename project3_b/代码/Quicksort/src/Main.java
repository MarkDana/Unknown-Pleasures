import java.util.Arrays;
import java.util.Random;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.ForkJoinTask;
import java.util.concurrent.RecursiveAction;


class Main extends RecursiveAction {

    private static final int SERIAL_THRESHOLD = 100;

    private final int[] a;
    private final int left;
    private final int right;

    public Main(int[] a) {
        this(a, 0, a.length - 1);
    }

    private Main(int[] a, int left, int right) {
        this.a = a;
        this.left = left;
        this.right = right;
    }

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

    private boolean serialThresholdMet() {
        return right - left < SERIAL_THRESHOLD;
    }

    private static int partition(int[] a, int left, int right) {
        // chose middle value of range for our pivot
        int pivotValue = a[middleIndex(left, right)];

        --left;
        ++right;

        while (true) {
            do
                ++left;
            while (a[left] < pivotValue);

            do
                --right;
            while (a[right] > pivotValue);

            if (left < right) {
                int tmp = a[left];
                a[left] = a[right];
                a[right] = tmp;
            } else {
                return right;
            }
        }
    }

    // calculates middle index without integer overflow
    private static int middleIndex(int left, int right) {
        return left + (right - left) / 2;
    }

    public static void main(String[] args) {

        int[] array = new int[20];

        for (int i = 0; i <= 19; i++) {
            array[i] = new Random().nextInt(100);
        }

        System.out.println(Arrays.toString(array));

        Main fork = new Main(array);
        ForkJoinPool pool = new ForkJoinPool();
        pool.invoke(fork);
        System.out.println(Arrays.toString(array));
    }

}