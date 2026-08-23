/**************************************************************************/
/*  export_plugin.cpp                                                     */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "export_plugin.h"

#include "core/config/project_settings.h"
#include "core/io/image_loader.h"
#include "editor/editor_node.h"
#include "editor/editor_paths.h"
#include "editor/editor_scale.h"
#include "editor/editor_string_names.h"
#include "editor/import/resource_importer_texture_settings.h"
#include "logo_svg.gen.h"
#include "run_icon_svg.gen.h"

#include "modules/modules_enabled.gen.h" // For svg.
#ifdef MODULE_SVG_ENABLED
#include "modules/svg/image_loader_svg.h"
#endif

static const char *NX_TEMPLATE_DEBUG = "nx_debug.elf";
static const char *NX_TEMPLATE_RELEASE = "nx_release.elf";

String EditorExportPlatformNX::get_name() const {
	return "Nintendo Switch";
}

String EditorExportPlatformNX::get_os_name() const {
	return "NX";
}

Ref<Texture2D> EditorExportPlatformNX::get_logo() const {
	return logo;
}

String EditorExportPlatformNX::_get_tool_path(const String &p_tool) {
	String tool = p_tool;
#ifdef WINDOWS_ENABLED
	tool += ".exe";
#endif

	String devkitpro = String(EDITOR_GET("export/nx/devkitpro")).strip_edges();
	if (devkitpro.is_empty()) {
		devkitpro = OS::get_singleton()->get_environment("DEVKITPRO").strip_edges();
	}

	if (!devkitpro.is_empty()) {
		String path = devkitpro.simplify_path().path_join("tools").path_join("bin").path_join(tool);
		if (FileAccess::exists(path)) {
			return path;
		}
	}

	return tool;
}

Error EditorExportPlatformNX::_run_tool(const String &p_tool, const List<String> &p_args, const String &p_stage) {
	String output;
	int exit_code = -1;

	Error err = OS::get_singleton()->execute(p_tool, p_args, &output, &exit_code, true);
	if (err != OK) {
		add_message(EXPORT_MESSAGE_ERROR, p_stage, vformat(TTR("Could not run \"%s\". Add the devkitPro tools to PATH, or set \"export/nx/devkitpro\" in the Editor Settings."), p_tool));
		return err;
	}
	if (exit_code != 0) {
		add_message(EXPORT_MESSAGE_ERROR, p_stage, vformat(TTR("\"%s\" exited with code %d:\n%s"), p_tool, exit_code, output));
		return ERR_CANT_CREATE;
	}

	return OK;
}

String EditorExportPlatformNX::_get_template_path(const Ref<EditorExportPreset> &p_preset, bool p_debug, String *r_error) const {
	String custom = String(p_preset->get(p_debug ? "custom_template/debug" : "custom_template/release")).strip_edges();

	if (!custom.is_empty()) {
		if (!FileAccess::exists(custom)) {
			if (r_error) {
				*r_error = vformat(TTR("Custom template not found: \"%s\"."), custom);
			}
			return String();
		}
		return custom;
	}

	return find_export_template(p_debug ? NX_TEMPLATE_DEBUG : NX_TEMPLATE_RELEASE, r_error);
}

Error EditorExportPlatformNX::_make_icon(const Ref<EditorExportPreset> &p_preset, const String &p_path) {
	String icon_path = String(p_preset->get("application/icon")).strip_edges();
	if (icon_path.is_empty()) {
		icon_path = String(GLOBAL_GET("application/config/icon")).strip_edges();
	}
	if (icon_path.is_empty()) {
		return ERR_SKIP;
	}

	Ref<Image> img;
	img.instantiate();
	if (ImageLoader::load_image(icon_path, img) != OK || img->is_empty()) {
		add_message(EXPORT_MESSAGE_WARNING, TTR("Create Icon"), vformat(TTR("Could not read the application icon \"%s\". The NRO will use the default icon."), icon_path));
		return ERR_SKIP;
	}

	img->convert(Image::FORMAT_RGB8);
	if (img->get_width() != 256 || img->get_height() != 256) {
		img->resize(256, 256, Image::INTERPOLATE_LANCZOS);
	}

	if (img->save_jpg(p_path, 0.9) != OK) {
		add_message(EXPORT_MESSAGE_WARNING, TTR("Create Icon"), vformat(TTR("Could not write the application icon to \"%s\". The NRO will use the default icon."), p_path));
		return ERR_SKIP;
	}

	return OK;
}

Error EditorExportPlatformNX::_make_romfs(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_romfs_dir) {
	Ref<DirAccess> da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
	ERR_FAIL_COND_V(da.is_null(), ERR_CANT_CREATE);

	if (!da->dir_exists(p_romfs_dir)) {
		Error err = da->make_dir_recursive(p_romfs_dir);
		if (err != OK) {
			add_message(EXPORT_MESSAGE_ERROR, TTR("Save PCK"), vformat(TTR("Could not create the RomFS directory \"%s\"."), p_romfs_dir));
			return err;
		}
	}

	String pck_path = p_romfs_dir.path_join("game.pck");
	if (FileAccess::exists(pck_path)) {
		da->remove(pck_path);
	}

	return save_pack(p_preset, p_debug, pck_path);
}

Error EditorExportPlatformNX::export_project(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, int p_flags) {
	ExportNotifier notifier(*this, p_preset, p_debug, p_path, p_flags);

	String template_error;
	String template_path = _get_template_path(p_preset, p_debug, &template_error);
	if (template_path.is_empty()) {
		add_message(EXPORT_MESSAGE_ERROR, TTR("Prepare Template"), template_error.is_empty() ? TTR("Export template not found.") : template_error);
		return ERR_FILE_NOT_FOUND;
	}

	Ref<DirAccess> da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
	ERR_FAIL_COND_V(da.is_null(), ERR_CANT_CREATE);
	if (!da->dir_exists(p_path.get_base_dir())) {
		add_message(EXPORT_MESSAGE_ERROR, TTR("Export"), vformat(TTR("The export path \"%s\" does not exist."), p_path.get_base_dir()));
		return ERR_FILE_BAD_PATH;
	}

	String tmp_dir = EditorPaths::get_singleton()->get_cache_dir().path_join("nx_export");
	String romfs_dir = tmp_dir.path_join("romfs");

	Error err = _make_romfs(p_preset, p_debug, romfs_dir);
	if (err != OK) {
		return err;
	}

	String title = String(p_preset->get("application/title")).strip_edges();
	if (title.is_empty()) {
		title = String(GLOBAL_GET("application/config/name")).strip_edges();
	}
	if (title.is_empty()) {
		title = "Godot Game";
	}

	String author = String(p_preset->get("application/author")).strip_edges();
	if (author.is_empty()) {
		author = "Unknown";
	}

	String version = String(p_preset->get("application/version")).strip_edges();
	if (version.is_empty()) {
		version = "1.0.0";
	}

	String nacp_path = tmp_dir.path_join("game.nacp");

	List<String> nacp_args;
	nacp_args.push_back("--create");
	nacp_args.push_back(title);
	nacp_args.push_back(author);
	nacp_args.push_back(version);
	nacp_args.push_back(nacp_path);

	err = _run_tool(_get_tool_path("nacptool"), nacp_args, TTR("Create NACP"));
	if (err != OK) {
		return err;
	}

	String icon_path = tmp_dir.path_join("icon.jpg");
	bool has_icon = _make_icon(p_preset, icon_path) == OK;

	String nro_path = p_path;
	if (nro_path.get_extension().to_lower() != "nro") {
		nro_path += ".nro";
	}

	List<String> nro_args;
	nro_args.push_back(template_path);
	nro_args.push_back(nro_path);
	nro_args.push_back("--nacp=" + nacp_path);
	if (has_icon) {
		nro_args.push_back("--icon=" + icon_path);
	}
	nro_args.push_back("--romfsdir=" + romfs_dir);

	err = _run_tool(_get_tool_path("elf2nro"), nro_args, TTR("Create NRO"));
	if (err != OK) {
		return err;
	}

	add_message(EXPORT_MESSAGE_INFO, TTR("Export"), vformat(TTR("Created \"%s\"."), nro_path));
	return OK;
}

void EditorExportPlatformNX::get_export_options(List<ExportOption> *r_options) const {
	r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "custom_template/debug", PROPERTY_HINT_GLOBAL_FILE, "*.elf"), ""));
	r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "custom_template/release", PROPERTY_HINT_GLOBAL_FILE, "*.elf"), ""));

	r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/title", PROPERTY_HINT_PLACEHOLDER_TEXT, "Application Title"), ""));
	r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/author", PROPERTY_HINT_PLACEHOLDER_TEXT, "Author Name"), ""));
	r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/version", PROPERTY_HINT_PLACEHOLDER_TEXT, "1.0.0"), "1.0.0"));
	r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/icon", PROPERTY_HINT_FILE, "*.png,*.jpg,*.jpeg,*.svg"), ""));

	r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "texture_format/etc2_astc"), true));
}

bool EditorExportPlatformNX::get_export_option_visibility(const EditorExportPreset *p_preset, const String &p_option) const {
	return true;
}

void EditorExportPlatformNX::get_preset_features(const Ref<EditorExportPreset> &p_preset, List<String> *r_features) const {
	r_features->push_back("arm64");
	if (p_preset.is_valid() && bool(p_preset->get("texture_format/etc2_astc"))) {
		r_features->push_back("etc2");
		r_features->push_back("astc");
	}
}

void EditorExportPlatformNX::get_platform_features(List<String> *r_features) const {
	r_features->push_back("nx");
	r_features->push_back("mobile");
}

void EditorExportPlatformNX::resolve_platform_feature_priorities(const Ref<EditorExportPreset> &p_preset, HashSet<String> &p_features) {
}

List<String> EditorExportPlatformNX::get_binary_extensions(const Ref<EditorExportPreset> &p_preset) const {
	List<String> extensions;
	extensions.push_back("nro");
	return extensions;
}

bool EditorExportPlatformNX::has_valid_export_configuration(const Ref<EditorExportPreset> &p_preset, String &r_error, bool &r_missing_templates, bool p_debug) const {
	String err;

	bool dvalid = exists_export_template(NX_TEMPLATE_DEBUG, &err);
	bool rvalid = exists_export_template(NX_TEMPLATE_RELEASE, &err);

	String custom_debug = String(p_preset->get("custom_template/debug")).strip_edges();
	if (!custom_debug.is_empty()) {
		dvalid = FileAccess::exists(custom_debug);
		if (!dvalid) {
			err += TTR("Custom debug template not found.") + "\n";
		}
	}

	String custom_release = String(p_preset->get("custom_template/release")).strip_edges();
	if (!custom_release.is_empty()) {
		rvalid = FileAccess::exists(custom_release);
		if (!rvalid) {
			err += TTR("Custom release template not found.") + "\n";
		}
	}

	bool valid = dvalid || rvalid;
	r_missing_templates = !valid;

	String devkitpro = String(EDITOR_GET("export/nx/devkitpro")).strip_edges();
	if (devkitpro.is_empty() && OS::get_singleton()->get_environment("DEVKITPRO").strip_edges().is_empty()) {
		err += TTR("devkitPro was not found. Set \"export/nx/devkitpro\" in the Editor Settings, or make sure that elf2nro and nacptool are in PATH.") + "\n";
	}

	if (!err.is_empty()) {
		r_error = err;
	}

	return valid;
}

bool EditorExportPlatformNX::has_valid_project_configuration(const Ref<EditorExportPreset> &p_preset, String &r_error) const {
	String err;
	bool valid = true;

	if (p_preset->get("texture_format/etc2_astc")) {
		if (!ResourceImporterTextureSettings::should_import_etc2_astc()) {
			valid = false;
		}
	}

	if (!err.is_empty()) {
		r_error = err;
	}

	return valid;
}

Ref<Texture2D> EditorExportPlatformNX::get_run_icon() const {
	return run_icon;
}

bool EditorExportPlatformNX::poll_export() {
	if (nxlink_pid != 0 && !OS::get_singleton()->is_process_running(nxlink_pid)) {
		nxlink_pid = 0;
		return true;
	}
	return false;
}

int EditorExportPlatformNX::get_options_count() const {
	return 1;
}

String EditorExportPlatformNX::get_option_label(int p_index) const {
	return nxlink_pid != 0 ? TTR("Stop nxlink") : TTR("Run on Switch (nxlink)");
}

String EditorExportPlatformNX::get_option_tooltip(int p_index) const {
	return TTR("Upload the exported NRO to a Nintendo Switch running the homebrew menu, using nxlink.");
}

Ref<ImageTexture> EditorExportPlatformNX::get_option_icon(int p_index) const {
	return nxlink_pid != 0 ? stop_icon : EditorExportPlatform::get_option_icon(p_index);
}

Error EditorExportPlatformNX::run(const Ref<EditorExportPreset> &p_preset, int p_device, int p_debug_flags) {
	if (nxlink_pid != 0) {
		OS::get_singleton()->kill(nxlink_pid);
		nxlink_pid = 0;
		return OK;
	}

	String nxlink = String(EDITOR_GET("export/nx/nxlink")).strip_edges();
	if (nxlink.is_empty()) {
		nxlink = _get_tool_path("nxlink");
	}

	String nro_path = EditorPaths::get_singleton()->get_cache_dir().path_join("nx_export").path_join("run.nro");

	Error err = export_project(p_preset, true, nro_path, p_debug_flags);
	if (err != OK) {
		return err;
	}

	List<String> args;
	args.push_back("-s");

	String host = String(EDITOR_GET("export/nx/nxlink_host")).strip_edges();
	if (!host.is_empty()) {
		args.push_back("-a");
		args.push_back(host);
	}

	args.push_back(nro_path);

	Vector<String> debug_flags;
	gen_debug_flags(debug_flags, p_debug_flags);
	if (!debug_flags.is_empty()) {
		args.push_back("--");
		for (int i = 0; i < debug_flags.size(); i++) {
			args.push_back(debug_flags[i]);
		}
	}

	err = OS::get_singleton()->create_process(nxlink, args, &nxlink_pid);
	if (err != OK) {
		nxlink_pid = 0;
		add_message(EXPORT_MESSAGE_ERROR, TTR("Run"), vformat(TTR("Could not run \"%s\". Set \"export/nx/nxlink\" in the Editor Settings."), nxlink));
		return err;
	}

	return OK;
}

void EditorExportPlatformNX::cleanup() {
	if (nxlink_pid != 0) {
		OS::get_singleton()->kill(nxlink_pid);
		nxlink_pid = 0;
	}
}

EditorExportPlatformNX::EditorExportPlatformNX() {
	if (EditorNode::get_singleton()) {
#ifdef MODULE_SVG_ENABLED
		Ref<Image> img = memnew(Image);
		const bool upsample = !Math::is_equal_approx(Math::round(EDSCALE), EDSCALE);

		ImageLoaderSVG::create_image_from_string(img, _nx_logo_svg, EDSCALE, upsample, false);
		logo = ImageTexture::create_from_image(img);

		ImageLoaderSVG::create_image_from_string(img, _nx_run_icon_svg, EDSCALE, upsample, false);
		run_icon = ImageTexture::create_from_image(img);
#endif

		Ref<Theme> theme = EditorNode::get_singleton()->get_editor_theme();
		if (theme.is_valid()) {
			stop_icon = theme->get_icon(SNAME("Stop"), EditorStringName(EditorIcons));
		} else {
			stop_icon.instantiate();
		}
	}
}
