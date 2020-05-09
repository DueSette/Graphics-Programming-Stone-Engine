#version 420

void main()
{             
    //this fragment shader simply needs to exist, but it can be completely empty (saves a lot of resources)
    //the color buffer for the currently bound framebuffer does not need to be updated as we are only extracting the depth
    //at this point, the depth is already retrieved and sent to the bound framebuffer
}  