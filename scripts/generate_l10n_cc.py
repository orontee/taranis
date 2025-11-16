#!/usr/bin/env python3

"""Generate C++ files from PO files.
"""

from pathlib import Path
from typing import Optional
import argparse
import re

from jinja2 import Environment, FileSystemLoader


def extract_language_from(file_path: Path) -> str:
    pattern = "([a-z]{2})\\.po"
    found = re.match(pattern, file_path.name)
    if not found:
        raise RuntimeError(f"Failed to extract language from {str(file_path)!r}")
    return found.group(1)


def identify_supported_languages(po_files: Path) -> list[str]:
    linguas_file = po_files / "LINGUAS"
    with open(linguas_file) as fh:
        return [lang.strip() for lang in fh.readlines()]


def load_translations(po_files: Path) -> dict[str, dict[str, str]]:
    translations: dict[str, dict[str, str]] = {}

    supported_languages = identify_supported_languages(po_files)

    for po_file in po_files.glob("*.po"):
        try:
            language = extract_language_from(po_file)
        except RuntimeError as error:
            print(str(error))
            continue

        if language not in supported_languages:
            print(f"Skipping translations for language {language!r}")
            continue

        print(f"Loading translations for language {language!r}")

        language_translations: dict[str, str] = {}

        def store_translation(msgid_tokens: list[str],
                              msgstr_tokens: list[str]):
            msgid = ("".join(msgid_tokens)
                     .replace('""', "")
                     .replace("\n", "")
                     .replace('"', "\""))
            if len(msgid) == 0:
                return

            msgstr = ("".join(msgstr_tokens)
                      .replace('""', "")
                      .replace("\n", "")
                      .replace('"', "\""))
            language_translations[msgid] = msgstr

        with open(po_file, "r") as fh:
            msgid_tokens: list[str] = []
            msgstr_tokens: list[str] = []
            defining: Optional[str] = None
            for line in fh:
                if line.startswith("#"):
                    continue

                if line.startswith("msgid "):
                    defining = "msgid"
                    msgid_tokens.clear()
                    msgid_tokens.append(line[6:].strip())
                elif line.startswith("msgstr "):
                    defining = "msgstr"
                    msgstr_tokens.clear()
                    msgstr_tokens.append(line[6:].strip())
                elif not line.startswith('"'):
                    if defining is None:
                        continue
                    defining = None
                    store_translation(msgid_tokens, msgstr_tokens)

                elif defining == "msgid":
                    msgid_tokens.append(line.strip())
                elif defining == "msgstr":
                    msgstr_tokens.append(line.strip())

            if defining is not None:
                store_translation(msgid_tokens, msgstr_tokens)

        translations[language] = language_translations

    return translations


def generate_file(translations: dict[str, dict[str, str]], *,
                  output_path: Path, template_path: Path):
    keys = sorted(list(set([
        key for language in translations for key in translations[language]
    ])))

    env = Environment(loader = FileSystemLoader(template_path.parent))
    template = env.get_template(template_path.name)

    content = template.render(keys=keys, translations=translations)
    with open(output_path, "w") as fh:
        fh.write(content)

    print(f"Template for l10n sources rendered to {str(output_path)!r}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("po_files", type=Path, help="Path to PO files")
    parser.add_argument("--output", type=Path, help="Path of file to generate")
    parser.add_argument("--template", type=Path, help="Path to template file")
    args = parser.parse_args()

    translations = load_translations(args.po_files)
    generate_file(translations, output_path=args.output, template_path=args.template)
