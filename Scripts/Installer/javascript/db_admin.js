use admin;
db.createUser({
    user: "admin",
    pwd: "admin",
    roles: [
    { role: "root", db: "admin" },
    { role: "dbOwner", db: "ProjectTemplates" },
    { role: "dbOwner", db: "ProjectsLargeData" },
    { role: "dbOwner", db: "System" },
    { role: "dbOwner", db: "UserSettings" },
    { role: "dbOwner", db: "Projects" }]
});