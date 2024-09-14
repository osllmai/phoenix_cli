"use client"; // Mark this component as a Client Component

import { useState } from 'react';
import { useRouter } from 'next/navigation';
import { Metadata } from 'next';
import { login } from '@/app/phoenix_services/auth';

import { Brand } from '@/components/ui/brand';
import { Input } from '@/components/ui/input';
import { Label } from '@/components/ui/label';
import { SubmitButton } from '@/components/ui/submit-button';



export default function Login({ searchParams }: { searchParams: { message: string } }) {
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  const router = useRouter();

  const signIn = async (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault();

    try {
      const { access_token, refresh_token, workspace } = await login(email, password);

      // Save tokens in local storage
      localStorage.setItem('access_token', access_token);
      localStorage.setItem('refresh_token', refresh_token);

      // Redirect to the home workspace
      if (workspace.is_home) {
        router.push(`/${workspace.id}/chat`);
      } else {
        throw new Error('Home workspace not found');
      }
    } catch (error) {
      console.error('Error during sign-in:', error);
      router.push(`/login?message=${(error as Error).message}`);
    }
  };

  return (
    <div className="flex w-full flex-1 flex-col justify-center gap-2 px-8 sm:max-w-md">
      <form
        className="animate-in text-foreground flex w-full flex-1 flex-col justify-center gap-2"
        onSubmit={signIn}
      >
        <Brand />

        <Label className="text-md mt-4" htmlFor="email">
          Email
        </Label>
        <Input
          className="mb-3 rounded-md border bg-inherit px-4 py-2"
          name="email"
          placeholder="you@example.com"
          value={email}
          onChange={(e) => setEmail(e.target.value)}
          required
        />

        <Label className="text-md" htmlFor="password">
          Password
        </Label>
        <Input
          className="mb-6 rounded-md border bg-inherit px-4 py-2"
          type="password"
          name="password"
          placeholder="••••••••"
          value={password}
          onChange={(e) => setPassword(e.target.value)}
          required
        />

        <SubmitButton className="mb-2 rounded-md bg-blue-700 px-4 py-2 text-white">
          Login
        </SubmitButton>

        {searchParams?.message && (
          <p className="bg-foreground/10 text-foreground mt-4 p-4 text-center">{searchParams.message}</p>
        )}
      </form>
    </div>
  );
}
