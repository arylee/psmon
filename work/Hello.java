import java.text.SimpleDateFormat;
import java.util.Date;

public class Hello {

    public static void main(String[] args) throws Exception {
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        while (true) {
            Date now = new Date();
            System.out.print(format.format(now));
            System.out.println("\tHello, World!");
            Thread.sleep(5000);
        }
    }

}