using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Newtonsoft.Json;
using Streamer.bot.Plugin.Interface;

using Settings = System.Collections.Generic.Dictionary<string, object>;

public class SettingsManager
{
    private const string SettingsPath = "C:/Users/Thomas/AppData/Local/EpiphaneBot";

    public class Scope
    {
        public Scope(SettingsManager manager, Settings settings)
        {
            this.manager = manager;
            this.settings = settings;
        }

        private readonly SettingsManager manager;
        private readonly Settings settings;

        public string Serialized
        {
            get
            {
                return JsonConvert.SerializeObject(settings, Formatting.Indented);
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
            manager.PersistSettings();
            return GetValue(key);
        }

        public bool Has(string key)
        {
            return settings.ContainsKey(key);
        }

        public object this[string key]
        {
            get => GetValue(key);
            set => SetValue(key, value);
        }
    }

    private string path = Path.Combine(SettingsPath, "Settings.json");

    // Inputs
    private IInlineInvokeProxy CPH;

    private Dictionary<string, Scope> scopes = new Dictionary<string, Scope>();
    private Dictionary<string, Settings> fullSettings = new Dictionary<string, Settings>();

    public SettingsManager(IInlineInvokeProxy CPH)
    {
        this.CPH = CPH;

        LoadSettings();
    }

    ~SettingsManager()
    {
        PersistSettings();
    }

    public void HandleDeserializationError(object sender, Newtonsoft.Json.Serialization.ErrorEventArgs errorArgs)
    {
        string currentError = errorArgs.ErrorContext.Error.Message;
        CPH.LogWarn($"JSON Error: {currentError}");
        errorArgs.ErrorContext.Handled = true;
    }

    private void LoadSettings()
    {
        CPH.LogDebug("Reloading settings");
        if (!File.Exists(path))
        {
            return;
        }

        using (StreamReader file = File.OpenText(path))
        {
            JsonSerializer serializer = new JsonSerializer();
            serializer.Error += HandleDeserializationError;
            fullSettings = (Dictionary<string, Settings>)serializer.Deserialize(file, typeof(Dictionary<string, Settings>));

            foreach (KeyValuePair<string, Settings> pairs in fullSettings)
            {
                scopes[pairs.Key] = new Scope(this, pairs.Value);
            }
        }

    }

    private void PersistSettings(int retries = 2)
    {
        CPH.LogDebug("Persisting settings to disk");
        using (StreamWriter file = File.CreateText(path))
        {
            file.Write(JsonConvert.SerializeObject(fullSettings, Formatting.Indented));
        }
    }

    public Scope GetScope(string name)
    {
        if (!fullSettings.ContainsKey(name))
        {
            fullSettings[name] = new Settings();
        }

        if (!scopes.ContainsKey(name))
        {
            scopes[name] = new Scope(this, fullSettings[name]);
        }

        return scopes[name];
    }

    public Scope this[string key]
    {
        get => GetScope(key);
    }
}
