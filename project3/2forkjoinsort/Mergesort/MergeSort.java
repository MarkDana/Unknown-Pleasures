import java.util.Arrays;
import java.util.Random;
import java.util.concurrent.*;
import java.util.Scanner;

public class MergeSort extends RecursiveAction {
    //eg. [x,y,z], left=0, right=2
    private int array[];
    private int left;
    private int right;
    private static final int THRESHOLD = 3;

    // MergeSort(int[] array, int left, int right) {
    //     this.array = array;
    //     this.left = left;
    //     this.right = right;
    //     System.out.println(String.format("creating sort %d to %d",left,right-1));
    // }

    MergeSort(int[] array, int low, int high) {
    this.array = array;
    this.low = low;
    this.high = high;
    System.out.println(String.format("creating sort %d to %d",low,high));
    // 
    }

    // public void compute() {
    //     int size = right - left + 1;
    //     System.out.println(String.format("left=%d, right=%d, size=%d",left,right,size));

        
    //     if (size <= 1)return;
    //     if (size <= THRESHOLD){
    //         Arrays.sort(array, left, right + 1);
    //         return;
    //     }//in java util, pass in the next index of ending
        
    //     int mid = (left+right)>>1;
    //     System.out.println(String.format("from %d to %d, mid=%d",left,right,mid));

        
    //     MergeSort sortLeft = new MergeSort(array, left, mid);
    //     MergeSort sortRight = new MergeSort(array, mid+1, right);
        
    //     sortLeft.fork();
    //     sortRight.fork();
    //     sortLeft.join();
    //     sortRight.join();

    //     // invokeAll(sortLeft,sortRight);
    //     merge(mid);
    // }

    public void compute() {
    int size = high - low;
    System.out.println(String.format("本线程负责第%d位到第%d位的数字",low,high-1));
    if (size <= THRESHOLD) { // 直接排序
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

    // public void merge(int mid) {
    //     System.out.println(String.format("merge [%d,%d], [%d,%d]",left,mid,mid+1,right));
    //     int[] temp = new int[right - left + 1];
    //     int i = left, j = mid + 1, k = 0;
    //     while(i <= mid && j <= right){
    //         if(array[i] <= array[j])temp[k++] = array[i++];
    //         else temp[k++] = array[j++];
    //     }
    //     while(i <= mid) temp[k++] = array[i++];
    //     while(j <= right) temp[k++] = array[j++];
    //     for (k = left; k <= right; ++k)array[left + k] = temp[k];
    // }

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

    // public static void main(String[] args) {

    //     int[] array = new int[20];

    //     for (int i = 0; i < 20; i++) {
    //         array[i] = new Random().nextInt(100);
    //     }

    //     System.out.println(Arrays.toString(array));

    //     ForkJoinPool pool = new ForkJoinPool();
    //     MergeSort fork = new MergeSort(array,0,array.length-1);
        
    //     pool.invoke(fork);
    //     System.out.println(Arrays.toString(array));
    // }

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


// import java.util.Arrays;
// import java.util.Random;
// import java.util.concurrent.*;

// class Main extends RecursiveAction {
//     private static final int SERIAL_THRESHOLD = 100;
//     private int array[];
//     private int left;
//     private int right;

//     public Main(int[] array, int left, int right) {
//         this.array = array;
//         this.left = left;
//         this.right = right;
//     }

//     *
//      * Inherited from RecursiveAction.
//      * Compare it with the run method of a Thread.
     
//     @Override
//     protected void compute() {
//         if (left < right) {
//             if (serialThresholdMet()) {
//                 Arrays.sort(array, left, right + 1);
//             }
//             else{
//                 int mid = (left + right) / 2;
//                 Main leftSort = new Main(array, left, mid);
//                 Main rightSort = new Main(array, mid + 1, right);
//                 invokeAll(leftSort, rightSort);
//                 merge(left, mid, right);
//             }
//         }
//     }

//     private boolean serialThresholdMet() {
//         return right - left < SERIAL_THRESHOLD;
//     }

//     private void merge(int left, int mid, int right) {
//         int temp [] = new int[right - left + 1];
//         int x = left;
//         int y = mid + 1;
//         int z = 0;
//         while (x <= mid && y <= right) {
//             if (array[x] <= array[y]) {
//                 temp[z] = array[x];
//                 z++;
//                 x++;
//             } else {
//                 temp[z] = array[y];
//                 z++;
//                 y++;
//             }
//         }
//         while (y <= right) {
//             temp[z++] = array[y++];
//         }
//         while (x <= mid) {
//             temp[z++] = array[x++];
//         }

//         for (z = 0; z < temp.length; z++) {
//             array[left + z] = temp[z];
//         }
//     }

//     public static void main(String[] args) {

//         int[] array = new int[200];

//         for (int i = 0; i <= 150; i++) {
//             array[i] = new Random().nextInt(100);
//         }

//         System.out.println(Arrays.toString(array));

//         Main fork = new Main(array,0,array.length-1);
//         ForkJoinPool pool = new ForkJoinPool();
//         pool.invoke(fork);
//         System.out.println(Arrays.toString(array));
//     }
// }