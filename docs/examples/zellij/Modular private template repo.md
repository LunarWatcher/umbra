# Modular, private template repo

While committing things directly to repos works, it's not great if you plan to work on multiple machines, or you just want to back it up somewhere safe. 

In your `.profile` or `.zprofile`, you can set
```bash
export $UMBRA_ZELLIJ_PRIVATE_SUBDIR=/path/to/your/private/template/repo/{{curr_folder}}
```
Where the contents of your template repo is
```
<root>
|+ .git
|+ some_project
    |- default.kdl
    |- other.kdl
    |- third.kdl
|+ umbra
    |- default.kdl
```

This gives you a centralised place to put all your templates, while still allowing for magic scoping.
