// prompts.ts
import api from './api';

export interface Prompt {
  id: number;
  content: string;
  workspace_id: number;
  user_id: string;
  created_at?: string;
  updated_at?: string;
  // Add other prompt fields as needed
}

export const getPromptsByWorkspaceId = async (workspaceId: string): Promise<Prompt[]> => {
  try {
    const response = await api.get<Prompt[]>(`/prompt/by-workspace/${workspaceId}`);
    return response.data;
  } catch (error: any) {
    throw new Error(error.response?.data?.message || 'Failed to fetch prompts');
  }
};
