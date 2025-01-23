import argparse
import sys

from scripts import format


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--dir",
        help="directory to run clang-format on (default: current directory)",
        default=".",
    )

    args = parser.parse_args()

    excludes = format.excludes_from_file(format.DEFAULT_CLANG_FORMAT_IGNORE)

    files = format.list_files(
        args.dir,
        exclude=excludes,
        extensions=args.extensions.split(","),
    )
    print(files)
    return 0


if __name__ == "__main__":
    sys.exit(main())
