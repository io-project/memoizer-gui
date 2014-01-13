import pl.edu.uj.tcs.memoizer.sharing.FacebookSharer;
import pl.edu.uj.tcs.memoizer.sharing.TwitterSharer;

import java.net.URL;

/**
 * Udostępnia bezpośrednio funkcjonalność wszystkich znanych implementacji {@link pl.edu.uj.tcs.memoizer.sharing.ISharer}
 *
 * @author Maciej Poleski
 */
public class MetaSharer {
    private final FacebookSharer facebookSharer;
    private final TwitterSharer twitterSharer;

    /**
     * Tworzy obiekt który będzie wykorzystywał podane obiekty to wykonywania żądań udostępniania.
     *
     * @param facebookSharer Obiekt {@link FacebookSharer} który będzie służył do udostępniania na Facebook-u.
     * @param twitterSharer  Obiekt {@link TwitterSharer} który będzie służył do udostępniania na Twitter-ze.
     */
    public MetaSharer(FacebookSharer facebookSharer, TwitterSharer twitterSharer) {
        this.facebookSharer = facebookSharer;
        this.twitterSharer = twitterSharer;
    }

    /**
     * Tworzy instancje przy użyciu domyślnych konstruktorów klas {@link FacebookSharer} i {@link TwitterSharer}.
     *
     * @return Instancja stworzona przy użyciu domyślnych parametrów.
     */
    public static MetaSharer defaultMetaSharer() {
        return new MetaSharer(new FacebookSharer(), new TwitterSharer());
    }

    /**
     * Udostępnia na Facebook-u.
     *
     * @param urlToObjectToShare Link który zostanie udostępniony
     */
    public void shareOnFacebook(URL urlToObjectToShare) {
        facebookSharer.share(urlToObjectToShare);
    }

    /**
     * Udostępnia na Twitter-ze.
     *
     * @param urlToObjectToShare Link który zostanie udostępniony.
     */
    public void shareOnTwitter(URL urlToObjectToShare) {
        twitterSharer.share(urlToObjectToShare);
    }
}
