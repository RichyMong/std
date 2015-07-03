public class Synchronized {
    private Integer x = new Integer(0);
    private Integer y = new Integer(0);
    private Object lockx = new Object();
    private Object locky = new Object();

    void incx() {
        synchronized (lockx) {
            ++x;
        }
    }

    void incy() {
        synchronized (locky) {
            ++y;
        }
    }

    public String toString() {
        return "x: " + x.toString() + ", y: " + y.toString();
    }

    static class Test extends Thread {
        private Synchronized consumer;

        Test(Synchronized consumer) {
            this.consumer = consumer;
        }

        public void run() {
            for (int i = 0; i != 10000; ++i) {
                consumer.incx();
                consumer.incy();
            }
        }
    }

    public static void main(String args[]) throws InterruptedException {
        Synchronized sync = new Synchronized();
        Synchronized.Test test = new Synchronized.Test(sync);
        test.start();
        for (int i = 0; i != 10000; ++i) {
            sync.incx();
            sync.incy();
        }
        test.join();
        System.out.println(sync);
    }
}
