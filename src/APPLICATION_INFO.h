#ifndef APPLICATION_INFO_H
#define APPLICATION_INFO_H

#define APPLICATION_VERSION "VERSION_HERE"
#define APPLICATION_GIT_COMMIT_HASH "GIT_COMMIT_HASH_HERE"
#define APPLICATION_GITHUB_REPO_URL "https://github.com/SebiAi/GlyphVisualizer"
#define APPLICATION_GITHUB_LATEST_RELEASE_API_URL "https://api.github.com/repos/SebiAi/GlyphVisualizer/releases/latest"
#define APPLICATION_CRASH_REPORTING_URL "https://github.com/SebiAi/GlyphVisualizer/issues/new/choose"
#define APPLICATION_GITHUB_COMMIT_URL QString("https://github.com/SebiAi/GlyphVisualizer/commit/%1").arg(APPLICATION_GIT_COMMIT_HASH)
#define APPLICATION_RELEASE_URL QString("https://github.com/SebiAi/GlyphVisualizer/releases/tag/%1")

#endif // APPLICATION_INFO_H
