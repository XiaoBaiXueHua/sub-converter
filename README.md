# subtitle converter
- only converts srt/ass files to vtt for now
- there's also almost no difference btwn srt and vtt files so none of the config options have anything to do with them.

## configuration options
in general you'd put the `name.config` files into the same folder as the `main.exe`.
### general
***configured in the `sub-converter.config` file.***

|name|type|default|explanation|
|--|--|--|--|
|***directory***|string|**`..`**|path for the subtitle source directory. relative to `main.exe`.|
|***output***|string|**` `**|path for the converter `.vtt` output directory. relative to `main.exe`. folder will be created if not already existent.|
|***csv***|string|**`tracklist.csv`**|(optional) csv with file & output information.|
|***file-type***|string|**`all`**|specifies whether the program will look for `.srt`, `.ass`, or both types of files.|
|***stylesheet***|bool|**`false`**|prints a stylesheet in the `.vtt` header.|
|***style-borders***|bool|**`false`**|default aegisub styles will often have 2px borders around their words. because these can often make for heavy file sizes, the styler will not print them by default.|
|***keep-gaps***|bool|**`false`**|when set to `TRUE`, cues will be made continuous by adding gaps between them in the output file(s).|
|***splitter-mode***|bool|**`false`**|splits a single `.ass` file into multiple different files depending on the dialogue style.|

currently, when `splitter-mode=false`, the file output will have each cue timed discretely, with all lines which overlapped in the `.ass` file at that point in time displayed in the order

### multi-file splitting
***configured in the `fanmix.config` file.*** <small>(this is mostly bc the first n biggest use i had for this project was for an annotated fanmix.)</small>

\*\*if you have `file-splitting=true`, then some of the settings in this file will take priority if set again.
some of these settings can also be overridden on a case-by-case basis while using a csv

|name|type|default|explanation|
|--|--|--|--|
|***make-html***|bool|**`false`**|makes an html file fragment for archiving the `.vtt` file. mostly intended for ao3.|
|***lyric-style***|string|**`Lyrics`**|name of the isolated style when splitting files. all other styles at this time will be combined into one file. *case-sensitive*.|
|***kara-fx***|string|**`kara`**|string used in the "effects" column of the `.ass` file to denote lines which are effectively k-timed.|
|***lyric-dir***|string|**`lyrics`**|directory name for the output of lyric tracks.<br />folder will be created if not already existent.|
|***comm-dir***|string|**`comm`**|directory name for the output of commentary (all other styles) tracks. this string will also be appended to the name of the file to prevent overwriting when output to the same dir as lyric file tracks.<br />folder will be created if not already existent.|
|***html-dir***|string|**`html`**|directory name for the output of html files.<br />folder will be created if not already existent.|
|***combined-track***|bool|**`false`**|create an extra combined track in the same style as when `splitter-mode=false`.|
|***combine-dir***|string|**`fanmixOpts::commDir`**|directory name for the output of combined tracks. will also append this and the commentary directory name strings to the filename as `${comm}-${combine}`|
|***mp3-host-url***|string|**` `**|file host name for systematically printing audio/video sources in the html file.|
|***mp3-host-ext***|string|**` `**|file host url extension for systematically printing audio/video sources in the html file.|
|***sub-host-url***|string|**` `**|same as `mp3-host-url` but for subtitles.|
|***sub-host-ext***|string|**` `**|same as `mp3-host-ext` but for subtitles.|

something important to note is that the first line of a series of kara-lines should *not* have the `kara` (or whatever chosen string) effect, so that the program may differentiate btwn discrete sequential kara line collections.

#### csv formatting

|col|name|details|
|--|--|--|
|***1***|title|**mandatory**<br />track title/file name. primarily used to locate the file in question from the source directory. when `make-html=true`, it is also used in printing the source link in the html output.|
|***2***|artist|**mandatory**<br />primarily used when printing the source link in the html output.|
|***3***|youtube|**mandatory**<br />primarily used when printing the fallback text in the html output as an alternate viewing/listening link.|
|***4***|alternate source|**optional**<br />other fallback source host urls for this particular track.|
|***5***|split type|**optional**<br />if set to `combined`, will produce an extra combined track, regardless of the configured settings.|
|***6***|extension|**optional**<br />specifies a file extension. useful if there are `.ass` and `.srt` files of the same name.|

i haven't actually gotten around to programming in the alternate sources, but when i someday do you can have more than one in the csv as long as they're separated by a space

## vtt output samples
<details><summary>view [lyrics/comm/combined]</summary>

### lyrics
```vtt
0:00:06.630 --> 0:00:11.370
Her turn signal blinked to the East

0:00:12.240 --> 0:00:15.730
towards the long wooden pier

0:00:15.730 --> 0:00:19.890
that hangs over the beach.

0:00:19.890 --> 0:00:22.440
She took off her dress

0:00:22.440 --> 0:00:26.100
and she walked towards the sea

0:00:26.100 --> 0:00:31.820
And thought, <i>maybe it could sweep away me</i>...
```
### comm
```vtt
cue-16
0:01:11.960 --> 0:01:14.320
1. the rlshp btwn savior n damned is different from that btwn peers

cue-17
0:01:14.320 --> 0:01:15.300
2. venti wants to be

kara-cue-17-0
0:01:15.300 --> 0:01:16.000
2. venti wants to be <i>loved </i>

kara-cue-17-1
0:01:16.000 --> 0:01:16.470
2. venti wants to be <i>loved as </i>

kara-cue-17-2
0:01:16.470 --> 0:01:16.900
2. venti wants to be <i>loved as a </i>

kara-cue-17-3
0:01:16.900 --> 0:01:17.240
2. venti wants to be <i>loved as a peer</i>!

gap-1
0:01:17.240 --> 0:01:21.040
 
```
- cues and gaps are numbered separately. karaoke numbering (as seen above) resets with every kara-group.
### combined
```vtt
cue-Lyr30-Anno26
0:01:43.850 --> 0:01:44.570
<v Lyrics>♪♫ (<i>(I know that he loves me, I know that he loves me)</i>) ♫♪</v>
<v echo>♪♫ (<i>(I, I...)</i>) ♫♪</v>
truthfully, i have a hard time fully and succinctly articulating my vision for the ideal xv rlshp. like i'm not going to say there's no platonic explanation for what they have going on bc (again) i'm aro i can believe lots of things can be platonic.
but also, i think that if they kissed abt their problems, it would fix smth abt both of them!

cue-Lyr30-Anno26
0:01:44.570 --> 0:01:47.480
<v Lyrics>♪♫ (<i>(I know that he loves me, I know that he loves me (×2))</i>) ♫♪</v>
truthfully, i have a hard time fully and succinctly articulating my vision for the ideal xv rlshp. like i'm not going to say there's no platonic explanation for what they have going on bc (again) i'm aro i can believe lots of things can be platonic.
but also, i think that if they kissed abt their problems, it would fix smth abt both of them!
```
- the lyric and annotation numbering theoretically should go up as they change but it's not perfect. good enough for now though.
</details>

### html output
when `file-splitting=true`, the html will only print the annotations track. kara-line collections are also consolidated into one cue.

many parts of the html output are still hard-coded. expect configurable options uhhhh Someday. they are printed as seen below, albeit with the variables replaced.

#### structure
```html 
<details class="annotations-controller" open><summary>&nbsp;</summary></details>
<video width="100%" controls="controls" controlslist="nodownload" playsinline="playsinline" crossorigin="anonymous" preload="metadata">
	hi. it appears that your browser does not support playing video in html5. wild, considering this day and age. <br />however, you can still listen to this song <a href="${csv_youtube}">on youtube</a>!
	<source src="${fanmixOpts_mp3-host-url}/${fanmixOpts_mp3-host-ext}/${trackNumber}-${csv_artist}-${csv_title}.mp3" type="audio/mp3">
	<source src="${csv_alternate-source}" type="audio/mp3">
	<track srclang="en" label="Annotations" kind="subtitles" src="${fanmixOpts_sub-host-url}/${fanmixOpts_sub-host-ext}/${fanmixOpts_comm-dir}/${csv_title}-${fanmixOpts_comm-dir}-${fanmixOpts_combine-dir}.vtt" default="default">
</video>
<hr />
<details>
	<summary><h3 class="file-declaration">WEBVTT</h3></summary>
<p></p>
<div class="scrolly">
<div class="cue"><h4>gap-0<br /><span class="timestamp">0:00:00.000 --&gt; 0:00:00.010</span></h4>
<p>&nbsp;</p></div>
<div class="cue"><h4>cue-1<br /><span class="timestamp">0:00:00.010 --&gt; 0:00:01.000</span></h4>
	<p>hi hewwo this is dummy text</p>
	<p>and it even prints <i><b>THE HTML</b></i> like in the <i>vtt file</i>.</p>
</div>
<div class="cue"><h4>cue-2<br /><span class="timestamp">0:00:01.000 --&gt; 0:00:07.180</span></h4>
	<p>hi this is another cue</p>
</div>
</div>
</details>
```
#### classes
`annotations-controller` -- this is the details element adjacent to the video. it does not hold the video element itself: it controls the visibility of the video using css.

`file-declaration` -- the h3 `WEBVTT` declaration in the cue details summary.

`scrolly` -- this is the div which holds all the separate cue divs.

`cue` -- this div contains a single, complete cue.

`timestamp` -- span with the start and end times for the cue.

#### other structural notes
- the empty paragraph immediately after the file-declaration summary is to provide a bit of space before the scrolling div that doesn't also scroll.
- when gaps are turned on, their cues will be printed as `<p>&nbsp;</p>`.
- line breaks within the same cue are converted into paragraphs.