// chats.ts
import api from './api';

export interface Chat {
  assistant_id: number;
  context_length: number;
  created_at: string;
  description: string;
  embeddings_provider: string;
  folder_id: number;
  id: number;
  include_profile_context: boolean;
  include_workspace_instructions: boolean;
  model: string;
  name: string;
  prompt: string;
  sharing: string;
  temperature: number;
  updated_at: string;
  user_id: string;
  workspace_id: number;
}

export const getChatsByWorkspaceId = async (workspaceId: string): Promise<Chat[]> => {
  try {
    const response = await api.get<Chat[]>(`/chat/by-workspace/${workspaceId}`);
    return response.data;
  } catch (error: any) {
    throw new Error(error.response?.data?.message || 'Failed to fetch chats');
  }
};
