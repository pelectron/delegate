import subprocess
import os
import argparse
import shutil
import datetime
from pathlib import Path


def temp_file_or_directory(base_path):
    suffix = datetime.datetime.now().strftime("%y%m%d_%H%M%S")
    path = Path(base_path)
    return Path(f"{path.parent}/{path.stem}_{suffix}{path.suffix}")


def build_doxygen_docs(
    doxygen: Path,
    doxyfile: Path,
    rootdir: Path,
    readme: Path,
    htmldir: Path,
    generate_html: bool,
    xml_dir: Path,
    generate_xml: bool,
):
    # create html directory if needed
    if generate_html:
        if not htmldir.exists():
            os.mkdir(htmldir)
    # create xml directory if needed
    if generate_xml:
        if not xml_dir.exists():
            os.mkdir(xml_dir)
    # doxyfile_override will be used as the actual input for doxygen.
    # This way, the original file is not modified.
    doxyfile_override = temp_file_or_directory(doxyfile)
    shutil.copy(doxyfile, doxyfile_override, follow_symlinks=True)

    # overriding the settings in temporary doxyfile
    with doxyfile_override.open("a") as file:
        file.write(f"OUTPUT_DIRECTORY = {rootdir}\n")
        file.write(f"INPUT = {rootdir.joinpath('include')} {rootdir.joinpath('example')} {readme}\n")
        file.write(f"EXAMPLE_PATH = {rootdir}/example\n")
        if generate_xml:
            file.write(f"XML_OUTPUT = {xml_dir}\n")
        else:
            file.write("GENERATE_XML = NO\n")
        if generate_html:
            file.write(f"HTML_OUTPUT = {htmldir}\n")
            file.write(
                "HTML_HEADER = "
                f"{rootdir}/subprojects/doxygen-dark-theme/html_header.html\n"
            )
            file.write(
                "HTML_FOOTER = "
                f"{rootdir}/subprojects/doxygen-dark-theme/html_footer.html\n"
            )
            file.write(
                "HTML_EXTRA_STYLESHEET = "
                f"{rootdir}/subprojects/doxygen-dark-theme/custom_dark_theme.css "
                f"{rootdir}/subprojects/doxygen-dark-theme/custom.css\n"
            )
        else:
            file.write("GENERATE_HTML = NO\n")
        file.write("USE_MDFILE_AS_MAINPAGE = {readme}")
    try:
        # run doxygen
        subprocess.run([str(doxygen), str(doxyfile_override)])
    except Exception as e:
        print(e)
    # deleting temporary doxyfile
    os.remove(doxyfile_override)


def generate_readme(readme_in: Path) -> Path:
    """'Generates doxygen compatible readme file from readme_in and returns its path."""
    prefixes = ["@mainpage\n", "@tableofcontents\n"]
    postfixes = [
        "\n[delegate]: #delegate-brief\n",
        "[multicast_delegate]: #multicast-delegate-brief\n",
    ]
    out = Path(str(readme_in)  + '.temp' ) # unique name for readme

    with open(readme_in, "r") as input, open(out, "w") as output:
        for p in prefixes:
            output.write(p)
        for line in input:
            output.write(line)
        for p in postfixes:
            output.write(p)
    return Path(out)


def markdown_docs(
    doxygen: Path,
    doxyfile: Path,
    doxybook2: Path,
    rootdir: Path,
    readme: Path,
    output_dir: Path,
    template_dir: Path,
    config_file: Path,
    xml_dir: Path,
    doxygen_already_run: bool,
):
    if not doxygen_already_run:
        build_doxygen_docs(
            doxygen=doxygen,
            doxyfile=doxyfile,
            rootdir=rootdir,
            readme=readme,
            htmldir=None,
            generate_html=False,
            xml_dir=xml_dir,
            generate_xml=True,
        )
    if not output_dir.exists():
        os.mkdir(output_dir)
    try:
        subprocess.run(
            [
                str(doxybook2),
                "-i",
                str(xml_dir),
                "-o",
                str(output_dir),
                "-t",
                str(template_dir),
                "--config",
                str(config_file),
            ]
        )
    except Exception as e:
        print(e)
    # cleanup for github emoji misrecognition in member function pointer syntax
    # The '&zwnj;' part stands for 'zero width non joining', a unicode code point.
    # for file in output_dir.iterdir():

    #     tmp = temp_file_or_directory(file)

    #     with open(file, "r") as inp, open(tmp, "w") as oup:
    #         for line in inp:
    #             oup.write(line.replace("::*)", "::&zwnj;*)"))
    #     os.remove(file)
    #     os.rename(tmp, file)
    for file in xml_dir.iterdir():
        os.remove(file)
    os.removedirs(xml_dir)


def build_both_docs(
    doxygen: Path,
    doxyfile: Path,
    doxybook2: Path,
    rootdir: Path,
    readme_in: Path,
    html_output: Path,
    markdown_output: Path,
    template_dir: Path,
    config_file: Path,
):

    readme = generate_readme(readme_in)

    xml_dir = rootdir.joinpath("tmp")

    build_doxygen_docs(
        doxygen=doxygen,
        rootdir=rootdir,
        doxyfile=doxyfile,
        readme=readme,
        htmldir=html_output,
        generate_html=True,
        xml_dir=xml_dir,
        generate_xml=True,
    )

    markdown_docs(
        doxygen=doxygen,
        doxyfile=doxyfile,
        doxybook2=doxybook2,
        rootdir=rootdir,
        readme=readme,
        output_dir=markdown_output,
        template_dir=template_dir,
        config_file=config_file,
        xml_dir=xml_dir,
        doxygen_already_run=True,
    )
    os.remove(readme)
    shutil.rmtree(xml_dir, ignore_errors=True)


def generate_github_readme(readme: Path, rootdir: Path, markdown_dir: Path) -> Path:
    outfile = rootdir.joinpath("readme.md")
    print(outfile)
    with open(readme, "r") as input, open(outfile, "w") as output:
        postfixes = [
            f"\n[delegate]: /{markdown_dir.stem}/classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md",
            f"\n[multicast_delegate]: /{markdown_dir.stem}/classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md",
            f"\n[max_storage_size]: /{markdown_dir.stem}/classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#max_storage_size\n",
        ]
        for line in input:
            output.write(line)
        for p in postfixes:
            output.write(p)


def build_markdown_only(
    doxygen: Path,
    doxyfile: Path,
    doxybook2: Path,
    rootdir: Path,
    readme_in: Path,
    markdown_output: Path,
    template_dir: Path,
    config_file: Path,
):

    readme = generate_readme(readme_in)

    xml_dir = to_abs(rootdir, temp_file_or_directory(Path("xml")))

    build_doxygen_docs(
        doxygen=doxygen,
        doxyfile=doxyfile,
        rootdir=rootdir,
        readme=readme,
        htmldir=None,
        generate_html=False,
        xml_dir=xml_dir,
        generate_xml=True,
    )

    markdown_docs(
        doxygen=doxygen,
        doxyfile=doxyfile,
        doxybook2=doxybook2,
        rootdir=rootdir,
        readme=readme,
        output_dir=markdown_output,
        template_dir=template_dir,
        config_file=config_file,
        xml_dir=xml_dir,
        doxygen_already_run=True,
    )
    os.remove(readme)


def to_abs(root, arg):
    p = Path(arg)
    return p if p.is_absolute() else Path(root) / p


def parse_arguments():
    parser = argparse.ArgumentParser(description="building delegate docs")
    parser.add_argument(
        "-doxygen",
        "--doxygen-path",
        help="path to doxygen executable",
        type=str,
        default="doxygen",
    )
    parser.add_argument(
        "-doxybook",
        "--doxybook-path",
        help="path to doxybook2 executable",
        type=str,
        default="doxybook2",
    )
    parser.add_argument(
        "-root", "--rootdir", help="project directory", type=str, default="."
    )
    parser.add_argument(
        "-html", "--htmldir", help="html output directory", type=str, default="docs"
    )
    parser.add_argument(
        "-r", "--readme", help="readme input file", type=str, default="readme.in.md"
    )
    parser.add_argument(
        "-md",
        "--markdown-dir",
        help="markdown output directory",
        type=str,
        default="markdown_docs",
    )
    parser.add_argument(
        "-t",
        "--templates",
        help="see doxybook2 templates",
        type=str,
        default="doxybook/templates",
    )
    parser.add_argument(
        "-c",
        "--config",
        help="see doxybook2 config",
        type=str,
        default="doxybook/config.json",
    )
    parser.add_argument("-doxyfile", type=str, default="Doxyfile")
    parser.add_argument(
        "-ghmd",
        "--generate-github-readme",
        help="generate github readme file",
        action="store_true",
    )
    parser.add_argument("--markdown-only", action="store_true")
    parser.add_argument("--doxygen-only", action="store_true")
    parser.add_argument("--verbose", action="store_true")
    args = parser.parse_args()
    if args.verbose:
      for arg in vars(args):
          print(arg, '=', getattr(args, arg))
    markdown_only = args.markdown_only
    doxygen_only = args.doxygen_only
    generate_gh_readme = args.generate_github_readme
    root = Path(args.rootdir)
    doxygen_path = (
        Path(args.doxygen_path)
        if args.doxygen_path != "doxygen"
        else Path(shutil.which("doxygen"))
    )
    doxybook_path = (
        Path(args.doxybook_path)
        if args.doxybook_path != "doxybook2"
        else Path(shutil.which("doxybook2"))
    )
    htmldir = to_abs(root, args.htmldir)
    markdown_dir = to_abs(root, args.markdown_dir)
    readme = to_abs(root, args.readme)
    templates = to_abs(root, args.templates)
    config = to_abs(root, args.config)
    doxyfile = to_abs(root, args.doxyfile)
    if not doxygen_path.exists():
        print("doxygen not found in path and not provided in command line arguments")
        quit()
    if not doxybook_path.exists():
        print("doxybook2 not found in path and not provided in command line arguments")
        quit()
    if not root.exists():
        print(f"invalid source directory '{root}'. Must be project root directory.")
        quit()
    if not readme.exists():
        print(f"{readme} is not a valid directory")
        quit()
    return (
        doxygen_path,
        doxybook_path,
        root,
        htmldir,
        markdown_dir,
        readme,
        markdown_only,
        doxygen_only,
        templates,
        config,
        doxyfile,
        generate_gh_readme,
    )

def main():
    (
        doxygen,
        doxybook,
        rootdir,
        htmldir,
        markdown_dir,
        readme,
        markdown_only,
        doxygen_only,
        templates,
        config,
        doxyfile,
        generate_gh_readme,
      ) = parse_arguments()
    if markdown_only:
        build_markdown_only(
            doxygen=doxygen,
            doxyfile=doxyfile,
            doxybook2=doxybook,
            rootdir=rootdir,
            readme_in=readme,
            markdown_output=markdown_dir,
            template_dir=templates,
            config_file=config,
        )
    elif doxygen_only:
        build_doxygen_docs(
            doxygen=doxygen,
            doxyfile=doxyfile,
            rootdir=rootdir,
            readme=readme,
            htmldir=htmldir,
            generate_html=True,
            xml_dir=None,
            generate_xml=False,
        )
    else:
        build_both_docs(
            doxygen=doxygen,
            doxyfile=doxyfile,
            doxybook2=doxybook,
            rootdir=rootdir,
            readme_in=readme,
            html_output=htmldir,
            markdown_output=markdown_dir,
            template_dir=templates,
            config_file=config,
        )
    if generate_gh_readme:
        generate_github_readme(readme, rootdir, markdown_dir)
# need to to the following things:
# 1. generate two versions of the readme. One for doxygen, one for github.
# 2. run doxygen
# 3. run doxybook2
# 4. go over markdown files and replace pesky '::*)' with '::&zwnj;*)'
#   (created by c++ member function pointer syntax). Github interprets this
#   sequence as an emoji.
if __name__ == "__main__":
    main()
