/* A Shell gives 4 KB of stack; libnix's startup sets this much up instead.
 * The AmiCradle Team. GPL 3, like Triplane Classic. */
unsigned long __stack = 256 * 1024;
