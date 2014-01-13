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

    public MetaSharer(FacebookSharer facebookSharer, TwitterSharer twitterSharer) {
        this.facebookSharer = facebookSharer;
        this.twitterSharer = twitterSharer;
    }

    public static MetaSharer defaultMetaSharer() {
        return new MetaSharer(new FacebookSharer(), new TwitterSharer());
    }

    public void shareOnFacebook(URL urlToObjectToShare) {
        facebookSharer.share(urlToObjectToShare);
    }

    public void shareOnTwitter(URL urlToObjectToShare) {
        twitterSharer.share(urlToObjectToShare);
    }
}
