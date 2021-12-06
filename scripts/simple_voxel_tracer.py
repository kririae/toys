# Simple Voxel Tracer
import math
import time
import taichi as ti
import numpy as np

ti.init(arch=ti.gpu)
res = 1280, 720
fov = 30  # unit: degree
inf = 1e10
max_ray_depth = 6
# 3D Vector field: [i, j][0|1|2]
color_buffer = ti.Vector.field(3, dtype=float, shape=res)
camera_pos = ti.Vector([0.0, 0.2, 10.0])
object_list = ti.Vector.field(3, dtype=float, shape=2)
object_list[0] = ti.Vector([0.0, 1.2, 0.0])
object_list[1] = ti.Vector([0.0, 0.1, 0.0])

norm_x = ti.Vector([1.0, 0.0, 0.0])
norm_y = ti.Vector([0.0, 1.0, 0.0])
norm_z = ti.Vector([0.0, 0.0, 1.0])
norm_list = ti.Vector.field(3, dtype=float, shape=6)
norm_list[0] = norm_x
norm_list[1] = -norm_x
norm_list[2] = norm_y
norm_list[3] = -norm_y
norm_list[4] = norm_z
norm_list[5] = -norm_z


@ti.func
def hit_object(p, pos, d) -> ti.i32:
    idx = 0
    size = 0.5

    _x1 = plane_inter(norm_x, p + size * norm_x, pos, d)
    _x2 = plane_inter(-norm_x, p - size * norm_x, pos, d)
    _y1 = plane_inter(norm_y, p + size * norm_y, pos, d)
    _y2 = plane_inter(-norm_y, p - size * norm_y, pos, d)
    _z1 = plane_inter(norm_z, p + size * norm_z, pos, d)
    _z2 = plane_inter(-norm_z, p - size * norm_z, pos, d)
    dist_list = ti.Vector([_x1, _x2, _y1, _y2, _z1, _z2])
    # print(_x1, _x2, _y1, _y2, _z1, _z2)

    _x_min, _x_max = min(_x1, _x2), max(_x1, _x2)
    _y_min, _y_max = min(_y1, _y2), max(_y1, _y2)
    _z_min, _z_max = min(_z1, _z2), max(_z1, _z2)
    # print(_x_min, _x_max, _y_min, _y_max, _z_min, _z_max)

    t_enter = max(_x_min, _y_min, _z_min)
    t_exit = min(_x_max, _y_max, _z_max)
    # print(t_enter, t_exit)

    for i in ti.static(range(6)):
        if t_enter == dist_list[i]:
            idx = i

    return t_enter < t_exit and t_exit > 0, t_enter, norm_list[idx]


@ti.func
def plane_inter(norm, p, pos, d) -> float:
    # Define a plane through its normal and a point
    # pos, d represents the ray
    t = (p - pos).dot(norm) / d.dot(norm)
    return t


@ti.func
def cast_ray(pos, d):
    obj_id = -1
    final_t = inf
    final_norm = ti.Vector([0.0, 0.0, 0.0])
    for i in ti.static(range(2)):
        inter, t, norm = hit_object(object_list[i], pos, d)
        if (inter and t < final_t):
            obj_id = i
            final_t = t
            final_norm = norm

    # hit ground
    t = plane_inter(norm_y,
                    ti.Vector([0.0, -0.5, 0.0]), pos, d)
    if (0 <= t < final_t):
        final_t = t
        final_norm = norm_y
    return final_t, final_norm, obj_id


@ti.func
def out_dir(n):
    u = ti.Vector([1.0, 0.0, 0.0])
    if abs(n[1]) < 1 - 1e-5:
        u = n.cross(ti.Vector([0.0, 1.0, 0.0])).normalized()
    v = n.cross(u)
    phi = 2 * math.pi * ti.random()
    ay = ti.sqrt(ti.random())
    ax = ti.sqrt(1 - ay**2)
    return ax * (ti.cos(phi) * u + ti.sin(phi) * v) + ay * n


@ti.kernel
def render():
    # print(cast_ray(camera_pos, ti.Vector([0, 0, -1.0]).normalized()))
    for u, v in color_buffer:
        aspect_ratio = res[0] / res[1]
        scale = ti.tan(0.5 * fov * math.pi / 180)
        x = (2.0 * (u + ti.random()) / res[0]) - 1
        y = (2.0 * (v + ti.random()) / res[1]) - 1
        x *= scale * aspect_ratio
        y *= scale
        d = ti.Vector([x, y, -1.0]).normalized()

        depth = 0
        pos = camera_pos
        output = ti.Vector([1.0, 1.0, 1.0])
        coeff = 0

        while depth < max_ray_depth:
            t, norm, obj_id = cast_ray(pos, d)
            depth += 1
            if obj_id == 1:  # ERROR
                coeff = 1
                depth = max_ray_depth
            else:
                if t != inf:
                    pos = pos + (t + 1e-4) * d
                    d = out_dir(norm)
                    output *= 0.6
                else:
                    depth = max_ray_depth
        color_buffer[u, v] += output * coeff


def main():
    gui = ti.GUI("Simple Voxel Tracer", res)
    interval = 100
    for i in range(100000):
        # Sampling
        render()
        if i % interval == 0:
            img = color_buffer.to_numpy() / (i + 1)
            gui.set_image(np.sqrt(img))
            gui.show()


if __name__ == '__main__':
    main()
