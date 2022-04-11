#!/usr/bin/python
# -*- coding: utf-8 -*-
"""Convert MIDI files to indexible mtree_array data types.

This module provides support for MIDI file convertation into indexible
mtree_array data types. The currently supported data type is float.

The module will find the Least Common Multiple of the note lengths in a whole
tone, then pad the array of pitches based on it:
    * If a note is a semitone, then it will appear LCM / 2 times in the array;
    * If a note is a third note, then it will appear LCM / 3 times in the array;
    * If a note is a quart, then it will appear LCM / 4 times in the array;
    * ...

Example:
    To use the module the user must specify the correct path to the MIDI
    file with the ``--midi-file`` option.

        $ python extract_data.py --midi-file=<path-to-file>
"""

from fractions import Fraction
from note_seq import midi_file_to_note_sequence
from numpy import unique
from math import lcm
from os import path
from sys import argv


def float_to_fraction(float_number: float) -> Fraction:
    """Converts a float number to a fraction.

    Args:
        float_number (float): A float number.

    Returns:
        Fraction: The float number converted to a fraction.
    """
    rounded_float = round(float_number, 2)
    if rounded_float == 0.33:
        return Fraction(1, 3)
    else:
        return Fraction(rounded_float)


def get_filepath():
    """Validates and returns the valid file path from the command line
    parameters.

    Raises:
        ValueError: If the number of parameters is not exactly one.
        ValueError: If the --midi-file=<path-to-file> option is not defined.
        ValueError: If the specified file does not exists.

    Returns:
        The valid filepath.
    """
    if len(argv) != 2:
        raise ValueError("The number of arguments must be exactly one!")
    if not argv[1].startswith("--midi-file="):
        raise ValueError("The --midi-file=<path-to-file> option is mandatory!")
    filepath = argv[1].split("=")[1]
    if not path.exists(filepath):
        raise ValueError(f"The specified file does not exists: {filepath}")
    return filepath


def get_lcm_of_note_lengths(note_lengths: "list[float]", fractions: "list[Fraction]") -> int:
    """Returns the Least Common Multiple of the note lengths.

    Args:
        note_lengths (list[float]): List of note_lengths (basically the
        list of differences between the start and end time of the notes)
        fractions (list[Fraction]): List of note lengths converted to Fraction
        data type.

    Returns:
        int: The Least Common Multiple of the note lenths.
    """
    return lcm(*[f.denominator for f in fractions])


if __name__ == "__main__":
    filepath = get_filepath()
    note_sequence = midi_file_to_note_sequence(filepath)
    pitches = [note.pitch for note in note_sequence.notes]
    note_lengths = [n.end_time - n.start_time for n in note_sequence.notes]
    fractions = [float_to_fraction(nl) for nl in note_lengths]
    lcm = get_lcm_of_note_lengths(note_lengths, fractions)
    output = []
    for i in range(0, len(pitches)):
        multiplier = int(lcm / fractions[i].denominator)
        for j in range(0, multiplier):
            output.append(pitches[i])
    print(f"Least Common Multiple of note lengths: {lcm}")
    print(f"Pitches: {output}")
