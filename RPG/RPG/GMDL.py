import bpy
import struct

def write_gmdl(self, context, filepath):
    print("running write_gmdl...")
    f = open(filepath, 'wb')
    
    #context = bpy.context
    scene = context.scene
    
    abs_filepath = bpy.path.abspath(filepath)
    
    texverts = []

    def clogVT(co,f):
        if co not in texverts:
            texverts.append(co)
            f.write(struct.pack('c',str.encode('u')))
            f.write(struct.pack('<2f',co[0],co[1]))
        f.write(struct.pack('c',str.encode('T')))
        vt = texverts.index(co)
        f.write(struct.pack('<I',vt))
    
    vertex_index_offset = 0
    for ob in bpy.context.scene.objects:
        if not ob.users_collection[0].hide_render and ob.type == 'MESH':
            mesh = ob.to_mesh(context.depsgraph, True)
            mesh.calc_normals_split()
            loop_normals = dict()
            for loop in mesh.loops:
                loop_normals[loop.vertex_index] = loop.normal;
            for col in mesh.vertex_colors:
                f.write(struct.pack('c',str.encode('c')))
                f.write(struct.pack('<i',len(col.name)))
                f.write(str.encode(col.name))
                f.write(struct.pack('<i',len(col.data)))
                for val in col.data:
                    f.write(struct.pack('<3f',val.color[0],val.color[1],val.color[2]))
            mat_faces = [[] for x in range(len(mesh.materials))]
            has_mat = len(mat_faces)>0
            if not has_mat:
                mat_faces.append([])
            for face in mesh.polygons:
                if (has_mat):
                    mat_faces[face.material_index].append(face)
                else:
                    mat_faces[0].append(face)
            for vert in mesh.vertices:
                f.write(struct.pack('c', str.encode('v')))
                final_co = ob.matrix_world @ vert.co
                f.write(struct.pack('<3f', final_co.x, final_co.y, final_co.z))
                norm = [0.0, 0.0, 0.0]
                try:
                    norm = loop_normals[vert.index]
                except:
                    norm = vert.normal
                if norm.x != 0.0 or norm.y != 0.0 or norm.z != 0.0:
                    f.write(struct.pack('c',str.encode('n')))
                    norm = ob.matrix_world.to_3x3() @ norm
                    f.write(struct.pack('<3f', norm.x, norm.y, norm.z))
                else:
                    try:
                        f.write(struct.pack('c',str.encode('n')))
                        norm = ob.matrix_world.to_3x3() @ vert.normal
                        f.write(struct.pack('<3f', norm.x, norm.y, norm.z))
                    except:
                        self.report({'INFO'}, 'No normal.')
                for group in vert.groups:
                    f.write(struct.pack('c',str.encode('w')))
                    f.write(struct.pack('i',group.group))
                    f.write(struct.pack('f',group.weight))
            for mat in mat_faces:
                f.write(struct.pack('c',str.encode('m')))
                if len(mat) > 0:
                    mat_data = mesh.materials[mat[0].material_index]
                    try:
                        for node in mat_data.node_tree.nodes:
                            if type(node) is bpy.types.ShaderNodeTexImage:
                                tex = node
                                f.write(struct.pack('c', str.encode('M')))
                                texture_filepath = tex.image.filepath
                                texture_abs_filepath = bpy.path.abspath(texture_filepath)
                                texture_filepath = bpy.path.relpath(texture_abs_filepath, abs_filepath)
                                texture_filepath = texture_filepath.replace("\\", "/");
                                texture_filepath = texture_filepath[5:]
                                f.write(struct.pack('<i', len(texture_filepath)))
                                f.write(str.encode(texture_filepath))
                                if tex.interpolation == 'Linear':
                                    f.write(struct.pack('c', str.encode('o')))
                                    f.write(struct.pack('<i', len("mag_linear")))
                                    f.write(str.encode("mag_linear"))
                                if tex.image.colorspace_settings.name == 'Linear':
                                    f.write(struct.pack('c', str.encode('o')))
                                    f.write(struct.pack('<i', len("cs_linear")))
                                    f.write(str.encode("cs_linear"))
                    except:
                        self.report({'INFO'}, 'No image texture.')
                for face in mat:
                    f.write(struct.pack('c',str.encode('F')))
                    f.write(struct.pack('<3I',face.vertices[0] + vertex_index_offset,face.vertices[1] + vertex_index_offset,face.vertices[2] + vertex_index_offset))
                    try:
                        for uv_layer in mesh.uv_layers:
                            fuvd = uv_layer.data
                            clogVT(fuvd[face.loop_indices[0]].uv,f)
                            clogVT(fuvd[face.loop_indices[1]].uv,f)
                            clogVT(fuvd[face.loop_indices[2]].uv,f)
                    except:
                        self.report({'ERROR'}, 'Texture error.')
                    f.write(struct.pack('c',str.encode('I')))
                    f.write(struct.pack('<3I',face.loop_indices[0],face.loop_indices[1],face.loop_indices[2]))
            vertex_index_offset += len(mesh.vertices)
            bpy.data.meshes.remove(mesh)
    
    f.close()

    return {'FINISHED'}

# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty


class ExportGMDL(bpy.types.Operator, ExportHelper):
    '''Exports a .gmdl file.'''
    bl_idname = "export.gmdl"
    bl_label = "Export GMDL"

    # ExportHelper mixin class uses this
    filename_ext = ".gmdl"

    filter_glob : StringProperty(default="*.gmdl", options={'HIDDEN'})

    @classmethod
    def poll(cls, context):
        return context.active_object != None

    def execute(self, context):
        return write_gmdl(self, context, self.filepath)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportGMDL.bl_idname, text="Game 3d Model (.gmdl)")


def register():
    bpy.utils.register_class(ExportGMDL)
    #bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportGMDL)
    #bpy.types.INFO_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()

    # test call
    bpy.ops.export.gmdl('INVOKE_DEFAULT')