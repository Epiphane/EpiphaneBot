using PetaPoco;
using Streamer.bot.Plugin.Interface;
using System;

[PrimaryKey("Id", AutoIncrement = false)]
public class User : IEquatable<User>
{
    public long Id { get; set; }
    public string Name { get; set; }
    public int Caterium { get; set; } = 0;
    public int Prestige { get; set; } = 0;

    public static void CreateTable(IDatabase DB)
    {
        DB.Execute(@"CREATE TABLE IF NOT EXISTS [User] (
            [Id] INTEGER,
            [Name] TEXT NOT NULL,
            [Caterium] INTEGER DEFAULT 0,
            [Prestige] INTEGER DEFAULT 0,
            PRIMARY KEY(""Id"")
        )");
    }

    public static void DeleteTable(IDatabase DB)
    {
        DB.Execute(@"DROP TABLE IF EXISTS [User]");
    }

    public static User Get(IInlineInvokeProxy CPH, IDatabase DB, long userId, string name)
    {
        if (!DB.Exists<User>(userId))
        {
            User newUser = new User()
            {
                Id = userId,
                Name = name,
                Caterium = 5,
                Prestige = 0,
            };

            DB.Insert(newUser);
        }

        return DB.Single<User>(userId);
    }

    public static User Find(IInlineInvokeProxy CPH, IDatabase DB, string name)
    {
        return DB.FirstOrDefault<User>("WHERE Name = @0 COLLATE NOCASE", name);
    }

    public override string ToString()
    {
        return Name;
    }

    public bool Equals(User other)
    {
        if (ReferenceEquals(other, null)) return false;
        if (ReferenceEquals(other, this)) return true;
        return Id == other.Id;
    }

    public override bool Equals(object obj)
    {
        if (ReferenceEquals(null, obj)) return false;
        if (ReferenceEquals(this, obj)) return true;

        return Equals(obj as User);
    }

    public override int GetHashCode()
    {
        return Name?.GetHashCode() ?? 0;
    }

    public static bool operator ==(User left, User right)
    {
        return Equals(left, right);
    }

    public static bool operator !=(User left, User right)
    {
        return !Equals(left, right);
    }
}
