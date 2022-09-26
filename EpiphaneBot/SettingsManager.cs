using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Newtonsoft.Json;
using Streamer.bot.Plugin.Interface;

using Settings = System.Collections.Generic.Dictionary<string, object>;

public class Setting<T>
{
    private readonly SettingsManager manager;
    private readonly Settings settings;
    private readonly string key;
    private readonly T defaultValue;

    public Setting(SettingsManager manager, Settings settings, string key, T defaultValue = default)
    {
        this.manager = manager;
        this.settings = settings;
        this.key = key;
        this.defaultValue = defaultValue;

        if (!settings.ContainsKey(key))
        {
            settings[key] = defaultValue;
            manager.PersistSettings();
        }
    }

    public static implicit operator T(Setting<T> s) => s.Get();

    public T Get()
    {
        return (T)(settings.ContainsKey(key) ? settings[key] : defaultValue);
    }

    public void Set(T value)
    {
        settings[key] = value;
        manager.PersistSettings();
    }
}

public class SettingsManager
{
    public const string SettingsPath = "C:/Users/Thomas/AppData/Local/EpiphaneBot";

    public class Scope
    {
        public Scope(SettingsManager manager, Settings settings)
        {
            this.manager = manager;
            this.settings = settings;
        }

        private readonly SettingsManager manager;

        private readonly Dictionary<string, Scope> scopes = new Dictionary<string, Scope>();
        private readonly Settings settings;

        public string Serialized
        {
            get
            {
                return JsonConvert.SerializeObject(settings, Formatting.Indented);
            }
        }

        public override string ToString()
        {
            return Serialized;
        }

        public T Get<T>(string key, T defaultValue = default)
        {
            object value = GetValue(key);
            if (value == null)
            {
                return defaultValue;
            }

            return (T)value;
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

        public Setting<T> At<T>(string key, T defaultValue = default)
        {
            return new Setting<T>(manager, settings, key, defaultValue);
        }

        public object this[string key]
        {
            get => GetValue(key);
            set => SetValue(key, value);
        }

        public Scope GetScope(string name, bool replace = true)
        {
            if (!settings.ContainsKey(name) || (replace && !(settings[name] is Settings)))
            {
                settings[name] = new Settings();
                manager.PersistSettings();
            }

            if (!(settings[name] is Settings))
            {
                throw new Exception($"Setting {name} is not a scope!");
            }

            if (!scopes.ContainsKey(name))
            {
                scopes[name] = new Scope(manager, (Settings)settings[name]);
            }

            return scopes[name];
        }
    }

    private string FilePath;
    public const string InMemory = ":memory:";
    public string PersistedSettings = "";
    public bool IsMemoryLocal
    {
        get { return FilePath == InMemory; }
    }

    // Inputs
    private readonly IInlineInvokeProxy CPH;

    private readonly Dictionary<string, Scope> scopes = new Dictionary<string, Scope>();
    private Dictionary<string, Settings> fullSettings = new Dictionary<string, Settings>();

    public SettingsManager(IInlineInvokeProxy CPH, string filePath = null)
    {
        this.CPH = CPH;

        if (filePath == null)
        {
            filePath = Path.Combine(SettingsPath, "Settings.json");
        }

        FilePath = filePath;
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

    public void ApplySettings(TextReader textReader)
    {
        JsonSerializer serializer = new JsonSerializer();
        JsonTextReader reader = new JsonTextReader(textReader);
        serializer.Error += HandleDeserializationError;
        fullSettings = serializer.Deserialize<Dictionary<string, Settings>>(reader);

        foreach (KeyValuePair<string, Settings> pairs in fullSettings)
        {
            scopes[pairs.Key] = new Scope(this, pairs.Value);
        }
    }

    public void LoadSettings()
    {
        if (IsMemoryLocal)
        {
            return;
        }

        CPH.LogDebug("Reloading settings");
        if (!File.Exists(FilePath))
        {
            return;
        }

        using (StreamReader file = File.OpenText(FilePath))
        {
            ApplySettings(file);
        }
    }

    public void PersistSettings(int retries = 2)
    {
        if (IsMemoryLocal)
        {
            PersistedSettings = JsonConvert.SerializeObject(fullSettings, Formatting.Indented);
            return;
        }

        CPH.LogDebug("Persisting settings to disk");
        using (StreamWriter file = File.CreateText(FilePath))
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
