# Julius input plugin

### Overview 

This plug in is intended to treat the quantization bit that is bigger than 16bit in Julius.


### build

```
gcc -lasound -lm -shared -o adin_mic_sph0645lm4h_alsa.jpi adin_mic_sph0645lm4h_alsa.c
cp adin_mic_sph0645lm4h_alsa.jpi plugin_dir/
```


### Julius setting

Add plugin dir and select an input type in myadin.jconf

```
.
.
-plugindir plugin_dir
-input myadin
.
.
```


