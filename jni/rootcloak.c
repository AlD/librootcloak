#define _GNU_SOURCE

// stat
#include <libgen.h>
#include <sys/stat.h>
#include <string.h>

// fopen
#include <stdio.h>

// readdir
#include <dirent.h>

// Required by all
#include <dlfcn.h>
#include <android/log.h>
#include <errno.h>


FILE *fopen(const char *path, const char *mode) {
	printf("In our own fopen, opening %s\n", path);
	__android_log_print(ANDROID_LOG_INFO, "ROOTCLOAK", "fopen(): path %s, mode %s", path, mode);

	char *fname = basename(path);

	if (strcasecmp("su", fname) == 0 || strcasecmp("daemonsu", fname) == 0 || strcasecmp("superuser.apk", fname) == 0) {
		__android_log_print(ANDROID_LOG_INFO, "ROOTCLOAK", "fopen(): Hiding su file %s", path);
		errno = ENOENT;
		return NULL;
	}

	static FILE *(*original_fopen)(const char*, const char*) = NULL;
	if (!original_fopen) {
		original_fopen = dlsym(RTLD_NEXT, "fopen");
	}
	return original_fopen(path, mode);
}


int stat(const char *path, struct stat *buf) {
	printf("In our own stat, stat()'ing %s\n", path);
	__android_log_print(ANDROID_LOG_INFO, "ROOTCLOAK", "stat(): path %s", path);

	char *fname = basename(path);

	if (strcasecmp("su", fname) == 0 || strcasecmp("daemonsu", fname) == 0 || strcasecmp("superuser.apk", fname) == 0) {
		__android_log_print(ANDROID_LOG_INFO, "ROOTCLOAK", "stat(): Hiding su file %s", path);
		errno = ENOENT;
		return -1;
	}


	static int (*original_stat)(const char*, struct stat*) = NULL;
	if (!original_stat) {
		original_stat = dlsym(RTLD_NEXT, "stat");
	}
	return (int) original_stat(path, buf);
}

int lstat(const char *path, struct stat *buf) {
	printf("In our own lstat, lstat()'ing %s\n", path);
	__android_log_print(ANDROID_LOG_INFO, "ROOTCLOAK", "stat(): path %s", path);

	char *fname = basename(path);

	if (strcasecmp("su", fname) == 0 || strcasecmp("daemonsu", fname) == 0 || strcasecmp("superuser.apk", fname) == 0) {
		__android_log_print(ANDROID_LOG_INFO, "ROOTCLOAK", "stat(): Hiding su file %s", path);
		errno = ENOENT;
		return -1;
	}


	static int (*original_lstat)(const char*, struct stat*) = NULL;
	if (!original_lstat) {
		original_lstat = dlsym(RTLD_NEXT, "lstat");
	}
	return (int) original_lstat(path, buf);
}

struct dirent *readdir(DIR *dirp) {
	printf("In our own readdir\n");
	__android_log_print(ANDROID_LOG_INFO, "ROOTCLOAK", "readdir()");

	static struct dirent *(*original_readdir)(DIR*);
	if (!original_readdir) {
		original_readdir = dlsym(RTLD_NEXT, "readdir");
	}

	struct dirent* ret = original_readdir(dirp);
	if (ret == NULL) {
		return ret;
	}

	printf("readdir(): d_name = %s\n", ret->d_name);
	__android_log_print(ANDROID_LOG_INFO, "ROOTCLOAK", "readdir(): d_name = %s", ret->d_name);

	unsigned int found = 0;
	do {
		if (strcasecmp("su", ret->d_name) == 0 || strcasecmp("daemonsu", ret->d_name) == 0 || strcasecmp("superuser.apk", ret->d_name) == 0) {
			printf("Found su file, reading next...");
			ret = original_readdir(dirp);
			printf(" done!\n");
			found = 1;
		} else {
			found = 0;
		}
	} while (found == 1 && ret != NULL);


	return ret;
}

int execl(const char *path, const char *arg, ...) {
    printf("In our own execl, execl()'ing %s\n", path);
    __android_log_print(ANDROID_LOG_INFO, "ROOTCLOAK", "execl(): path %s", path);

    char *fname = basename(path);

    if (strcasecmp("su", fname) == 0 || strcasecmp("daemonsu", fname) == 0 || strcasecmp("superuser.apk", fname) == 0) {
        __android_log_print(ANDROID_LOG_INFO, "ROOTCLOAK", "execl(): Hiding su file %s", path);
        errno = ENOENT;
        return -1;
    }


    static int (*original_execl)(const char *path, const char *arg, ...) = NULL;
    if (!original_execl) {
        original_execl = dlsym(RTLD_NEXT, "execl");
    }
    return (int) original_execl(path, arg);
}

char *strstr (const char *haystack, const char *needle) {
    printf("In our own strstr, haystack %s, needle %s\n", haystack, needle);
    __android_log_print(ANDROID_LOG_INFO, "ROOTCLOAK", "strstr(): haystack %s, needle %s", haystack, needle);

    static char *(*original_strcasestr)(const char*, const char*) = NULL;
    if (!original_strcasestr) {
        original_strcasestr = dlsym(RTLD_NEXT, "strcasestr");
    }

    if (strcasecmp("su", needle) == 0 || original_strcasestr(needle, "eu.chainfire.supersu") == 0) {
        __android_log_print(ANDROID_LOG_INFO, "ROOTCLOAK", "strstr(): Hiding su %s", haystack);
        return NULL;
    }

    static char *(*original_strstr)(const char*, const char*) = NULL;
    if (!original_strstr) {
        original_strstr = dlsym(RTLD_NEXT, "strstr");
    }
    return original_strstr(haystack, needle);
}

char *strcasestr (const char *haystack, const char *needle) {
    printf("In our own strcasestr, haystack %s, needle %s\n", haystack, needle);
    __android_log_print(ANDROID_LOG_INFO, "ROOTCLOAK", "strcasestr(): haystack %s, needle %s", haystack, needle);

    static char *(*original_strcasestr)(const char*, const char*) = NULL;
    if (!original_strcasestr) {
        original_strcasestr = dlsym(RTLD_NEXT, "strcasestr");
    }

    if (strcasecmp("su", needle) == 0 || original_strcasestr(needle, "eu.chainfire.supersu") == 0) {
        __android_log_print(ANDROID_LOG_INFO, "ROOTCLOAK", "strcasestr(): Hiding su %s", haystack);
        return NULL;
    }

    return original_strcasestr(haystack, needle);
}
