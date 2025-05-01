# Building

1. Download the Discord Game SDK from Discord's Developer website
2. Copy the lib/ and c/ directories to vendor/discord_game_sdk

```
vendor/
    discord_game_sdk/
        c/
        lib/
```

2. Set your architecture in CMakeLists.txt (x86, x86_64 or aarch64)
3. Build with CMake
