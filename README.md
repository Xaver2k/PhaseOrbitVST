# Phase Orbit 0.1 — Windows VST3 project

Status: früher Prototyp. Windows-x64-VST3-Build und Audiokern-Tests am 13.11.2026 erfolgreich. Oberfläche, MIDI-Routing und Betrieb in Ableton sind noch nicht im Host geprüft. Die Vorabversion unter Releases enthält das kompilierte Plugin; der Repository-Quellcode allein ist kein installierbares Plugin.


## Installation des fertigen Downloads

1. Unter [Releases](https://github.com/Xaver2k/PhaseOrbitVST/releases) das fertige Windows-Plugin-ZIP herunterladen und vollständig entpacken. Die GitHub-ZIPs „Source code“ sind nur für die Entwicklung.
2. Den **äußeren Ordner `Phase Orbit.vst3` samt Contents-Unterordner** nach `C:\Program Files\Common Files\VST3` kopieren. Nicht nur die innere gleichnamige Binärdatei kopieren. Keine ZIPs, Quellcode-Archive, README, .lib oder .exp in den VST3-Ordner kopieren.
3. Ableton öffnen: Einstellungen -> Plug-Ins -> VST3-Systemordner verwenden aktivieren und neu scannen. Die Bezeichnungen hängen von Version und Sprache ab.
4. Im Ableton-Browser unter Plug-Ins nach **Phase Orbit** suchen und auf eine Audiospur ziehen. Bei einer MIDI-Spur muss der Effekt hinter einem Instrument stehen.
5. Die Oberfläche über das Schraubenschlüssel-Symbol des Plugin-Geräts öffnen.

Das Plugin ist ein **Audioeffekt**, kein Klangerzeuger und keine eigenständig startbare Windows-App. Ohne laufendes Audiosignal hört man nichts. Nicht per Doppelklick auf die .vst3 starten.

## Erster Klangtest

Musik, einen Synthesizer oder einen Drumloop abspielen. Abhörlautstärke zunächst niedrig einstellen. Alle vier Makros auf **0.50** stellen; **Bypass aus**, **Phaser an**, **Delay FX aus**. Dann:

| Regler | Startwert |
| --- | --- |
| Rate | 0.35 Hz |
| Depth | 0.70 |
| Feedback | 0.25 |
| Centre | 800 Hz |
| Stereo | 0.50 |
| Mix | 0.50 |
| Output | 0 dB |

Das ist ein vorgeschlagener Ausgangspunkt, kein gehörtes oder geprüftes Preset. Rate verändern: schneller/langsamer schwebender Phaser. Centre verändern: Klangbewegung zu tieferen/höheren Frequenzen verschieben. Mix auf 0 ergibt das Originalsignal, auf 1 nur den Effekt.

Für Chorus-/Flanger-artige Klänge **Phaser aus**, **Delay FX an**, **Delay etwa 5 ms**, **Feedback zunächst 0.10–0.25**, **Mix 0.50**. Kürzere Delay-Zeiten und höheres Feedback können metallischere Flanger-Klänge erzeugen. Beide Effekte lassen sich kombinieren. Feedback vorsichtig erhöhen; das Plugin hat keinen Ausgangslimiter.

## Regler und Tasten erklärt

| Bedienelement | Wirkung |
| --- | --- |
| Rate | Geschwindigkeit der periodischen Modulation in Hz; nicht zum Songtempo synchronisiert. |
| Depth | Stärke der Frequenz-/Delay-Modulation, 0–1. |
| Feedback | Rückkopplung für einen ausgeprägteren/resonanteren Klang; negative Werte ändern die Rückkopplungspolarität. |
| Centre | Mittlere Phaser-Frequenz in Hz. Wirkt nur auf den Phaser. |
| Delay | Grundverzögerung in ms. Wirkt auf Delay FX. |
| Stereo | Phasenversatz der Modulation zwischen links/rechts; auf Mono-Eingang keine Stereo-Verbreiterung. |
| Mix | Verhältnis Original/Effekt: 0 = trocken, 1 = nur Effekt. |
| Output | Ausgangspegel in dB. |
| Phaser / Delay FX | Jeweilige Effektsektion ein-/ausschalten. |
| Bypass | Originalsignal durchlassen; auch Output wird umgangen. |
| Pegelbalken rechts | Ausgangsspitzen; orange zeigt Werte über digitalem Vollpegel an. Kein Limiter. |

Regler mit gedrückter Maustaste auf-/abwärts ziehen. Zahlenfeld zur direkten Eingabe verwenden. Doppelklick auf einen Regler stellt seinen Standardwert wieder her. Die große Wellenlinie ist dekorativ, keine gemessene Frequenzkurve oder Audioanzeige.

## Makros verwenden

Die vier unteren Regler sind vereinfachte Steuerungen. Im Dropdown darunter lässt sich das Ziel auswählen:

| Makro | Standardziel |
| --- | --- |
| Motion | Rate |
| Space | Stereo |
| Intensity | Feedback |
| Blend | Mix |

**0.50 ist neutral.** Größere Werte erhöhen und kleinere Werte senken das gewählte Ziel relativ zum oberen Basisregler. Der obere Regler bewegt sich dabei optisch nicht mit. Beispiel: Blend auf 0.50 belassen und oben Mix einstellen; danach mit Blend den Effektanteil relativ verändern. Makrobereiche sind normalisiert, keine festen Hz-/dB-Schritte. Mehrere Makros auf demselben Ziel addieren sich bis zur Bereichsgrenze. Unassigned deaktiviert die Zuweisung.

Version 0.1 unterstützt ein Ziel pro Makro. Ziele sind die acht kontinuierlichen Regler; Schalter können direkt per MIDI-Learn angesprochen werden. Makroziele selbst sind keine automatisierbaren Host-Parameter.

## Wenn nichts passiert

- Kein Ton: Läuft Audio auf dieser Spur? Bei MIDI-Spuren steht ein Instrument vor dem Effekt?
- Kein Effekt: Bypass aus, mindestens eine Sektion an und Mix über 0; Blend zunächst auf 0.50.
- Plugin nicht sichtbar: gesamten äußeren .vst3-Ordner prüfen, VST3-Systemordner aktivieren, neu scannen und ggf. Live neu starten. Windows-x64-Build, kein 32-Bit-Plugin.
- MIDI reagiert nicht: MIDI muss beim Effekt ankommen; siehe MIDI-Anleitung unten.
- Tastatur reagiert nicht: freien Plugin-Hintergrund anklicken. Textfelder, Ableton und die Computertastatur-MIDI-Funktion können Tasten abfangen.
- Einstellungen dauerhaft behalten: Ableton-Live-Set nach Änderungen speichern. Wiederherstellung einschließlich MIDI-Zuweisungen ist implementiert, aber noch nicht in Live getestet.


## Funktionen
- 8-stufiger Allpass-Phaser mit moduliertem Frequenzzentrum.
- Modulierter Delay-Effekt für Chorus-/Flanger-artige Klänge, 0,2–30 ms Grundverzögerung.
- Rate 0,02–12 Hz, Tiefe, bipolares Feedback, Stereo-LFO-Phasenversatz, Dry/Wet und Output.
- Phaser und Delay einzeln schaltbar, serieller Signalweg: Phaser -> Delay -> Mix/Output.
- Vier zuweisbare Makros; jedes steuert einen der acht kontinuierlichen Effektparameter. 50 % = neutral, darunter/darüber ein additiver Offset im normalisierten Parameterbereich. Der Basisregler bleibt sichtbar unverändert. Kein Mehrfachziel pro Makro in Version 0.1.
- MIDI-Learn für alle 15 Parameter: CC steuert absolut, Note-On toggelt zwischen Minimum/Maximum. Omni-Empfang auf allen MIDI-Kanälen, CC 0–127; kein Pitchbend, NRPN oder relativer Encoder-Modus. MIDI-Zuweisungen und Makroziele werden mit dem Plugin-Zustand gespeichert.
- Alle Regler, Makros und Schalter stehen dem Host als Automationsparameter zur Verfügung.
- Dunkle Oberfläche mit Pegel-/Übersteuerungsanzeige. Die Wellenlinie ist dekorativ und kein Audio-Oszilloskop.
- Mono/Stereo, Parameter-Glättung. Kein Tempo-Sync in dieser Version.

## Weg A: Auf Windows bauen
1. Visual Studio 2022 Community/Build Tools installieren; Workload **Desktopentwicklung mit C++**, einschließlich MSVC x64, Windows SDK und CMake auswählen. Git for Windows installieren.
2. ZIP vollständig entpacken. `build-windows.bat` ausführen. Internet wird zum Abrufen von JUCE 8.0.6 benötigt.
3. Nach erfolgreichem Build liegt das vollständige Plugin-Paket hier:
   `build/PhaseOrbit_artefacts/Release/VST3/Phase Orbit.vst3`
4. Gesamten Ordner `Phase Orbit.vst3` nach `C:\Program Files\Common Files\VST3` kopieren; dafür kann Windows Adminrechte anfordern. Das Skript installiert oder überschreibt selbst keine Plugins.
5. Ableton: VST3-Systemordner aktivieren und Plugins neu scannen. `Phase Orbit` auf eine Audiospur ziehen. Bei der ersten Klangprüfung mit niedrigem Abhörpegel starten: hohes Feedback kann Pegelspitzen erzeugen; dies ist kein Limiter.

## Weg B: Ohne lokale Build-Werkzeuge
Den Inhalt dieses Projektordners einschließlich `.github` in ein eigenes GitHub-Repository hochladen. `CMakeLists.txt` muss in dessen Wurzel liegen. Workflow unter Actions -> Windows VST3 -> Run workflow ausführen. Nach erfolgreichem Lauf Artifact `PhaseOrbit-Windows-x64-VST3` herunterladen und wie oben installieren. Ein erfolgreicher Windows-Build ist bereits verfügbar. Artefakte werden derzeit sieben Tage aufbewahrt; später den Workflow erneut ausführen. Actions-Nutzung unterliegt dem eigenen GitHub-Tarif.

## MIDI und Ableton
MIDI-Learn: Ziel im Dropdown auswählen, MIDI Learn drücken, CC-Regler bewegen oder Note senden. Erneutes Drücken bricht Learn ab. Clear MIDI entfernt ALLE MIDI-Zuweisungen.

Ableton muss MIDI an den Audioeffekt weiterleiten. Dafür eine MIDI-Spur anlegen, Controller als Eingang wählen, Monitoring auf In stellen und als MIDI-Ausgang die Audiospur mit Phase Orbit sowie das Plugin als Unterziel wählen (Bezeichnungen sind Live-versionsabhängig). Falls Live das Plugin nicht als MIDI-Ziel anbietet, kann die Integration erst nach Prüfung des Builds/der verwendeten Live-Version angepasst werden. Alternativ die Plugin-Parameter über Lives Configure-Funktion verfügbar machen und Lives eigene MIDI-Zuweisung verwenden.

Makros lassen sich ebenso über Lives Automation oder Rack-Makros steuern. Plugin-MIDI-Learn und Lives MIDI-Mapping sind zwei getrennte Möglichkeiten. Die direkte VST3-Parameterzuweisung im Audiothread muss mit dem konkreten Host validiert werden.

## Tastatur
Nach einem Klick auf den freien Hintergrund der Oberfläche:
- Q/W/E/R erhöht Makro 1/2/3/4 um 2 Prozentpunkte.
- A/S/X/F senkt Makro 1/2/3/4 um 2 Prozentpunkte.
- P toggelt Phaser, D toggelt Delay, B toggelt Bypass.

Die Kürzel sind fest und gelten nur bei Fokus auf der Plugin-Oberfläche. Host und fokussierte Text-/Reglerfelder können Eingaben abfangen. Keine globalen Windows-Hotkeys. Für Steuerung bei geschlossener Oberfläche Lives Key-Mapping verwenden.

## Teststatus und Grenzen
`tests/engine_test.cpp`: Stille, extreme Parameteränderungen einschließlich Feedback ±0,9, Mono/Stereo und 22,05/44,1/48/96/192 kHz, Dry-Mix und Bypass geprüft. Vollständiger Windows-x64-VST3-Build und derselbe DSP-Test unter Windows erfolgreich durchgeführt; noch kein Host-Test. Kein behaupteter Hörtest. Vor Produktiveinsatz sind Plugin-Validator, GUI-Prüfung sowie Tests von Automation, Save/Reload und MIDI in Live erforderlich. Momentan keine Werks-Presets, kein Installer, kein Signieren.

## Abhängigkeiten
JUCE 8.0.6 wird per CMake FetchContent geladen, nicht mitgeliefert. JUCE-Lizenz und VST3-Bedingungen vor Weitergabe/Veröffentlichung prüfen. Referenz: https://github.com/juce-framework/JUCE/tree/8.0.6 ; CMake-Dokumentation: https://github.com/juce-framework/JUCE/blob/8.0.6/docs/CMake%20API.md . Projektlizenz: AGPL-3.0-only (siehe LICENSE). JUCE wird unter der AGPLv3 genutzt. Abhängigkeiten behalten ihre jeweiligen Lizenzen. Bei Weitergabe von Binärdateien den vollständigen zugehörigen Quellcode dieses Versionsstands und die Lizenztexte mitliefern.

## Ableton-Referenz

https://help.ableton.com/hc/de/articles/209071729-Hinweise-zur-Nutzung-von-VST-Plug-Ins-unter-Windows
