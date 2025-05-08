#include <stdio.h>
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <limits.h>
#include <libgen.h>
#include <sys/stat.h>

#define CLASSIC 1
#define NATIVE 0
#define DF_PATH_MAX 4096

void Daemonize(const char daemonpath[DF_PATH_MAX], const char daemonname[DF_PATH_MAX], int fflag)
{
    char daemonfile[DF_PATH_MAX];
    snprintf(daemonfile, DF_PATH_MAX, "/etc/systemd/system/%s.service", daemonname);
    FILE *sysd = fopen(daemonfile, "w+");
    if (!sysd)
    {
        perror("Failed to open systemd service file");
        exit(1);
    }

    fprintf(sysd,
            "[Unit]\n"
            "Description:%s Daemon (Powered by DaemonForge) \n"
            "After=network.target\n"
            "\n\n"
            "[Service]\n",
            daemonname

    );
    if (fflag == CLASSIC)
    {
        fprintf(sysd,
                "Type=forking\n");
    }
    fprintf(sysd,
            "ExecStart=%s\n"
            "Restart=always\n"
            "User=%s\n"
            "StandardOutput=append:/var/log/%s.log\n"
            "StandardError=append:/var/log/%s.err\n"
            "\n\n"
            "[Install]\n"
            "Wantedby=multi-user.target\n",
            daemonpath, getenv("USER"), daemonname, daemonname);

    fclose(sysd);
    chmod(daemonfile, 0644);

    // Reload systemd to recognize the new .service file
    system("systemctl daemon-reload");

    // Enable the service so it starts on boot
    char enable_cmd[128];
    snprintf(enable_cmd, sizeof(enable_cmd), "systemctl enable %s.service", daemonname);
    system(enable_cmd);

    // Start the service immediately
    char start_cmd[128];
    snprintf(start_cmd, sizeof(start_cmd), "systemctl start %s.service", daemonname);
    system(start_cmd);

    printf("✅ Service '%s' installed and started via systemd.\n", daemonname);
}

int main(int argc, char *argv[])
{

    int fflag = NATIVE;
    char daemonpath[DF_PATH_MAX], daemonname[DF_PATH_MAX];
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s [-f] <exec_path> [-pv] [service_name]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int index = 1;
    if (strcmp(argv[index], "-f") == 0)
    {
        fflag = CLASSIC;
        index++;
    }

    if (realpath(argv[index], daemonpath) == NULL)
    {
        perror("Failed to resolve absolute path");
        exit(1);
    }

    index++;
    if (strcmp(argv[index], "-pv") == 0)
    {
        char oldpath[DF_PATH_MAX];
        strncpy(oldpath, daemonpath, DF_PATH_MAX);
        const char *venv = getenv("VIRTUAL_ENV");
        if (!venv)
        {
            fprintf(stderr, "❌ Error: VIRTUAL_ENV not set. Please activate your virtual environment before using -pv.\n");
            exit(1);
        }

        snprintf(daemonpath, DF_PATH_MAX, "%s/bin/python %s", venv, oldpath);

        index++;
    }
    if (argc > index)
    {
        snprintf(daemonname, DF_PATH_MAX, "%s", argv[index]);
    }
    else
    {
        char *base = basename(daemonpath);
        char *dot = strrchr(base, '.');
        if (dot && strcmp(dot, ".py") == 0)
        {
            *dot = '\0'; // strip .py
        }
        strncpy(daemonname, base, DF_PATH_MAX);
    }
    Daemonize(daemonpath, daemonname, fflag);
    return 0;
}