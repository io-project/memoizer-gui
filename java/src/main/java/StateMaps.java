import net.sf.json.JSONArray;
import pl.edu.uj.tcs.memoizer.gui.MainWindow;
import pl.edu.uj.tcs.memoizer.serialization.StateMap;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

/**
 * Ta klasa udostępnia kilka narzędzi ułatwiających prace z obiektami klasy {@link pl.edu.uj.tcs.memoizer.serialization.StateMap}
 *
 * @author Maciej Poleski
 */
public class StateMaps {
    public static String[] getSelectedPlugins(StateMap stateMap) {
        JSONArray array = MainWindow.getJsonArray(stateMap);
        String[] result = new String[array.size()];
        for (int i = 0; i < result.length; ++i)
            result[i] = array.getString(i);
        return result;
    }

    public static void setPluginsSelection(StateMap stateMap, String[] selected, String[] unselected) {
        Set<String> currentPluginsSet = new HashSet<>(Arrays.asList(getSelectedPlugins(stateMap)));
        currentPluginsSet.removeAll(Arrays.asList(unselected));
        currentPluginsSet.addAll(Arrays.asList(selected));
        MainWindow.storeSelectedPluginsList(stateMap, new ArrayList<>(currentPluginsSet));
    }
}
