# Haven of The Embraced

Haven of The Embraced (Haven) is a text-based Multi-User Dungeon (MUD) style game, based on the ROM code-base. Haven was created by 'Ugha' and is actively maintained by the HavenMUD team.

![Login Screen](https://github.com/Haven-of-the-Embraced/src/assets/6600917/43a5e93e-8e33-4b8a-baf9-47a326d8e94f)
![MOTD](https://github.com/Haven-of-the-Embraced/src/assets/6600917/24f99fe9-db57-4f0c-a494-6208841932cd)



## Compiling

Haven is written in C and requires gcc or an equivalent C compiler. Compilation is as simple as cloning the repository and invoking:

```bash
make new
```
Running `make new` will clean up the old obj files first.

## Run Haven

Haven of The Embraced will not run without the associated /data and /area files, which were intentionally not provided in this repository. Those files can be recreated using the provided code as a guide for their contents.

![Normal](https://github.com/Haven-of-the-Embraced/src/assets/6600917/2157a104-6963-4d9e-ba96-018d59505763)

```bash
./rom [port] [testrun]
```
Port: Any port above 1024 is acceptable. If a port is not specified, it defaults to 4000

Testrun: This option exists for CI/CD testing purposes. Invoking Haven with option testrun will cause the MUD to boot, complete three game_loops, and shutdown. 

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change. Issues labeled "Low Hanging Fruit" are relatively simple projects, great for someone just getting into MUD coding.

### Procedure

1. Fork the repository
2. Create your Feature Branch 
    * Descriptive names are encouraged. 
    * If you are coding a fix to formatting on 'who', a branch called 'whoformat' would be appropriate
3. Commit your changes to the feature branch
4. Open a Pull Request

## License
Haven of The Embraced is covered under multiple Licenses. ROM, Diku and MERC licenses apply to code derived from those sources.

Code written by the HavenMUD team are published under [CC BY-SA 2.0](https://creativecommons.org/licenses/by-sa/2.0/legalcode).

In brief, this means that you are free to share, edit, rework, rebuild, and redistribute this code in any fashion, so long as you follow the required licenses. In order to freely do so, you must give appropriate credit for the code (Included attribution headers in source files, a helpfile with attribution, etc) AND you must release your contributions under the same licenses as the code you received.
