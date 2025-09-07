#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>

typedef struct
{
    int show_line_numbers;
    int icase;
    int invert;
    int count_only;
    int force_filename;
    int recursive;
    int fixed;
    int extended;
} Options;

typedef struct
{
    const char *pattern;
    regex_t re;
    int regex_ready;
} Matcher;

static int matcher_init() {}
static void matcher_free(Matcher *matcher) {}
static int matcher_match_line() {}

typedef struct
{
    Options *opt;
    Matcher *matcher;
    int multiple_files;
    int exit_status;
} Context;

static int is_dir(const char *path)
{
    struct stat st;
    if (stat(path, &st) != 0)
        return 0;
    return S_ISDIR(st.st_mode);
}

static void process_stream(Context *ctx, const char *label, FILE *fp)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t n;
    unsigned long lineno = 0;
    unsigned long matches = 0;

    while ((n = getline(&line, &len, fp)) != -1)
    {
        lineno++;
        if (ctx->matcher->pattern == NULL)
            continue;
        int ok = matcher_match_line(ctx->matcher, ctx->opt, line);
        if (ok)
        {
            matches++;
            if (ctx->opt->count_only)
            {
                if (ctx->opt->force_filename || ctx->multiple_files)
                {
                    fprintf(stdout, "%s:", label);
                }

                if (ctx->opt->show_line_numbers)
                {
                    fprintf(stdout, "%lu:", lineno);
                }

                fputs(line, stdout);
            }
        }
    }

    if (ferror(fp))
    {
        fprintf(stderr, "Error reading from %s\n", label);
        clearerr(fp);
    }

    if (ctx->opt->count_only)
    {
        if (ctx->opt->force_filename || ctx->multiple_files)
        {
            fprintf(stdout, "%s:", label);
        }
        else
        {

            fprintf(stdout, "%lu\n", matches);
        }
    }

    free(line);
}

static void recurse_dir(Context *ctx, const char *path)
{
}

static void process_path(Context *ctx, const char *path)
{
    if (ctx->opt->recursive && is_dir(path))
    {
        recurse_dir(ctx, path);
        return;
    }

    if (strcmp(path, "-") == 0)
    {
        process_stream(ctx, "(standard input)", stdin);
        return;
    }

    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        fprintf(stderr, "Cannot open %s\n", path);
        return;
    }

    process_stream(ctx, path, fp);
    fclose(fp);
}

static void usage(const char *progname)
{
    fprintf(stderr,
            "Usage: %s [OPTIONS] PATTERN [FILE...]\n"
            "Options:\n"
            "  -n          Show line numbers\n"
            "  -i          Case insensitive matching\n"
            "  -v          Invert match\n"
            "  -c          Count matching lines\n"
            "  -H          Always print filename headers\n"
            "  -r          Recursive search in directories\n"
            "  -F          Fixed string matching\n"
            "  -E          Extended regular expressions\n",
            progname);
}

int main(int argc, char *argv[])
{
    Options opt = {0};
    int ch;
    while ((ch = getopt(argc, argv, "nivcHrFE")) != -1)
    {
        switch (ch)
        {
        case 'n':
            opt.show_line_numbers = 1;
            break;
        case 'i':
            opt.icase = 1;
            break;
        case 'v':
            opt.invert = 1;
            break;
        case 'c':
            opt.count_only = 1;
            break;
        case 'H':
            opt.force_filename = 1;
            break;
        case 'r':
            opt.recursive = 1;
            break;
        case 'F':
            opt.fixed = 1;
            break;
        case 'E':
            opt.extended = 1;
            break;
        default:
            usage(argv[0]);
            return 2;
        }
    }

    if (optind >= argc)
    {
        usage(argv[0]);
        return 2;
    }

    const char *pattern = argv[optind++];
    int file_count = argc - optind;

    Matcher matcher;
    if (matcher_init())
    {
        return 2;
    }

    Context ctx = {
        .opt = &opt,
        .matcher = &matcher,
        .multiple_files = file_count > 1 || opt.recursive,
        .exit_status = 1,
    };

    if (file_count == 0)
    {
        process_stream(&ctx, "(standard input)", stdin);
    }
    else
    {
        for (int i = optind; i < argc; i++)
        {
            process_path(&ctx, argv[i]);
        }
    }

    matcher_free(&matcher);
    return ctx.exit_status;
}