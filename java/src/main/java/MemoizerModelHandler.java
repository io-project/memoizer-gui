import pl.edu.uj.tcs.memoizer.gui.models.IMemoizerModelHandler;
import pl.edu.uj.tcs.memoizer.plugins.Meme;

/**
 * Handler na który model zgłasza dostępność nowych Mem-ów.
 *
 * @author Maciej Poleski
 */
public class MemoizerModelHandler implements IMemoizerModelHandler {
    // ABI
    private final long id;

    /**
     * Nie ma sensu tworzyć obiektów tej klasy.
     *
     * @param id Identyfikator słóżący do identyfikacji instancji po stronie natywnej.
     */
    private MemoizerModelHandler(long id) {
        this.id = id;
    }

    @Override
    public native void notifyUpdate(Meme meme, int id);

    @Override
    public native void notifyStreamEnd();
}
