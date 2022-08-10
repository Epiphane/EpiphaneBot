using PetaPoco;
using Streamer.bot.Plugin.Interface;

[PrimaryKey("Id", AutoIncrement = false)]
public class User
{
    public long Id { get; set; }
    public string Name { get; set; }
    public int Experience { get; set; } = 0;
    public int Wood { get; set; } = 0;
    public int Stone { get; set; } = 0;
    public int Iron { get; set; } = 0;
    public int Rubies { get; set; } = 0;
    public int Gold { get; set; } = 0;

    public static void CreateTable(IDatabase DB)
    {
        DB.Execute(@"CREATE TABLE IF NOT EXISTS [User] (
            [Id] INTEGER PRIMARY KEY,
            [Name] TEXT NOT NULL,
            [Experience] INTEGER DEFAULT 0,
            [Wood] INTEGER DEFAULT 0,
            [Stone] INTEGER DEFAULT 0,
            [Iron] INTEGER DEFAULT 0,
            [Rubies] INTEGER DEFAULT 0,
            [Gold] INTEGER DEFAULT 0
        )");
    }

    public static User Get(IInlineInvokeProxy CPH, IDatabase DB, long userId, string name)
    {
        if (!DB.Exists<User>(userId))
        {
            User newUser = new User()
            {
                Id = userId,
                Name = name
            };

            DB.Insert(newUser);
        }

        return DB.Single<User>(userId);
    }
}
