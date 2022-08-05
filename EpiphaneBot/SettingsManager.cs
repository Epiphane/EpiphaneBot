using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Newtonsoft.Json;

public class SettingsManager
{
    private const string SettingsPath = "C:/Users/Thomas/AppData/Local/EpiphaneBot";

    private string path = Path.Combine(SettingsPath, "Settings.json");

    private Dictionary<string, object> settings = new Dictionary<string, object>();

    public SettingsManager()
    {
        LoadSettings();
    }

    ~SettingsManager()
    {
        PersistSettings();
    }

    private void LoadSettings()
    {
        if (!File.Exists(path))
        {
            return;
        }

        using (StreamReader file = File.OpenText(path))
        {
            JsonSerializer serializer = new JsonSerializer();
            settings = (Dictionary<string, object>)serializer.Deserialize(file, typeof(Dictionary<string, object>));
        }

    }

    private void PersistSettings()
    {
        using (StreamWriter file = File.CreateText(path))
        {
            file.Write(JsonConvert.SerializeObject(settings, Formatting.Indented));
        }
    }

    public object GetValue(string key)
    {
        if (settings.ContainsKey(key))
        {
            return settings[key];
        }

        return null;
    }

    public object SetValue(string key, object value)
    {
        settings[key] = value;
        PersistSettings();
        return GetValue(key);
    }

    public bool Has(string key)
    {
        return settings.ContainsKey(key);
    }

    public object this[string key] {
        get => GetValue(key);
        set => SetValue(key, value);
    }
}
