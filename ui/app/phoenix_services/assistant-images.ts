// assistant-images.ts
import api from './api';

export const getAssistantImage = async (imagePath: string): Promise<string> => {
  try {
    // Assuming your backend provides an endpoint to get the image URL or data
    const response = await api.get<{ url: string }>(`/assistant/image`, {
      params: { path: imagePath },
    });
    return response.data.url;
  } catch (error: any) {
    throw new Error(error.response?.data?.message || 'Failed to fetch assistant image');
  }
};
